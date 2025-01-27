/*
 * m8ec - Embedded Client for the Dirtywave M8 Headless device.
 * Copyright (C) 2023 Lukas Neverauskis https://github.com/lukasnee
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

// Copyright 2021 Jonne Kokkonen
// Released under the MIT licence, https://opensource.org/licenses/MIT

#include "m8ec/m8/protocol.hpp"

#include "ILI9341/ili9341_gfx.h"
#include "m8ec/m8ec.hpp" // LOG
#include "m8ec/periph/UsbCdc.hpp"
#include "m8ec/slip.h"

#include "usb_host.h" // TODO decouple

#include <cstdio>
#include <cstring>

namespace m8ec::m8::protocol {
namespace cmd {

const char *id_to_name(uint8_t cmd_id);
template <typename Tcmd, uint8_t Tcmd_id, size_t Tcmd_size_min, size_t Tcmd_size_max> struct Cmd {
public:
    static constexpr uint8_t cmd_id = Tcmd_id;
    static constexpr size_t cmd_size_min = Tcmd_size_min;
    static constexpr size_t cmd_size_max = Tcmd_size_max;
    static bool validate_size(uint32_t actual_size) {
        if (actual_size < cmd_size_min || actual_size > cmd_size_max) {
            if (m8ec::Config::debug_m8_protocol) {
                LOG(err_fmt_invalid_packet_length_range, id_to_name(cmd_id), cmd_size_min, cmd_size_max, actual_size);
            }
            return false;
        }
        return true;
    }

private:
    static constexpr const char *err_fmt_invalid_packet_length_range =
        "Error: %s: Invalid packet length: expected [%u %u], got %lu\n";
};
#pragma pack(push, 1)
struct KeyState : public Cmd<KeyState, 0xFB, 2, 2> {
    struct Payload {
        std::uint8_t key_state;
        std::uint8_t unknown; // TODO figure out what this is
    } payload;
};
struct DrawWaveform
    : public Cmd<DrawWaveform, 0xFC, sizeof(Waveform::color), sizeof(Waveform::color) + sizeof(Waveform::buffer)> {
    Waveform waveform;
};
struct DrawCharacter : public Cmd<DrawCharacter, 0xFD, sizeof(Character), sizeof(Character)> {
    Character character;
};
struct DrawRectangle : public Cmd<DrawRectangle, 0xFE, sizeof(Rectangle), sizeof(Rectangle)> {
    Rectangle rectangle;
};
struct PrintSystemInfo : public Cmd<SystemInfo, 0xFF, sizeof(SystemInfo), sizeof(SystemInfo)> {
    SystemInfo system_info;
};
#pragma pack(pop)

const char *id_to_name(uint8_t cmd_id) {
    switch (cmd_id) {
    case KeyState::cmd_id:
        return "KeyState";
    case DrawWaveform::cmd_id:
        return "DrawWaveform";
    case DrawCharacter::cmd_id:
        return "DrawCharacter";
    case DrawRectangle::cmd_id:
        return "DrawRectangle";
    case PrintSystemInfo::cmd_id:
        return "SystemInfo";
    default:
        return "Unknown";
    }
}

} // namespace cmd

Service::Service(Service::Display &display)
    : fonas::Thread("m8svc", 3 * 1024 + Config::slip_buffer_size, 1), display(display) {}

bool Service::init() { return this->fonas::Thread::Start(); }

void Service::enable_display() {
    uint8_t buf[] = {'E'};
    if (!m8ec::periph::UsbCdc::get_instance().write(buf, 1)) {
        LOG("m8::protocol::enable_display failed\n");
    }
    LOG("m8::protocol::enable_display\n");
}

void Service::reset_display() {
    uint8_t buf[] = {'R'};
    if (!m8ec::periph::UsbCdc::get_instance().write(buf, 1)) {
        LOG("m8::protocol::reset_display failed\n");
    }
    LOG("m8::protocol::reset_display\n");
}

void Service::send_keys_state(Keys::State keys_state) {
    uint8_t buf[2] = {'C', keys_state.underlying};
    if (!m8ec::periph::UsbCdc::get_instance().write(buf, 2)) {
        LOG("m8::protocol::send_keys_state failed\n");
    }
}

// TODO separate out cmd parsing from the service into m8ec/m8
void Service::Run() {
    uint8_t slip_buffer[Config::slip_buffer_size];
    const slip_descriptor_s slip_descriptor = {
        .buf = slip_buffer,
        .buf_size = sizeof(slip_buffer),
        .recv_ctx = this,
        .recv_message = [](uint8_t *data, uint32_t size, void *recv_ctx) -> int {
            auto *service = reinterpret_cast<Service *>(recv_ctx);
            if (size < 1) {
                return 0;
            }
            const uint8_t cmd_id = data[0];
            const uint8_t *payload_data = &data[1];
            const uint32_t payload_size = size - 1;
            if (cmd_id == cmd::KeyState::cmd_id && cmd::KeyState::validate_size(payload_size)) {
                const auto &payload = reinterpret_cast<const cmd::KeyState *>(payload_data)->payload;
                if (m8ec::Config::debug_m8_protocol) {
                    LOG("KeyState: s:%02x,?:%02x\n", payload.key_state, payload.unknown);
                }
            }
            else if (cmd_id == cmd::DrawWaveform::cmd_id && cmd::DrawWaveform::validate_size(payload_size)) {
                const auto &waveform = reinterpret_cast<const cmd::DrawWaveform *>(payload_data)->waveform;
                const auto waveform_width = static_cast<uint16_t>(payload_size - sizeof(Color));
                if (m8ec::Config::debug_m8_protocol) {
                    LOG("DrawWaveform: w:%hu\n", waveform_width);
                }
                service->display.draw_waveform(waveform, waveform_width);
            }
            else if (cmd_id == cmd::DrawCharacter::cmd_id && cmd::DrawCharacter::validate_size(payload_size)) {
                const auto &character = reinterpret_cast<const cmd::DrawCharacter *>(payload_data)->character;
                if (m8ec::Config::debug_m8_protocol) {
                    LOG("DrawCharacter: c:'%c'(0x%02X)@x:%u,y:%u\n", character.c, character.c, character.pos.x,
                        character.pos.y);
                }
                service->display.draw_character(character);
            }
            else if (cmd_id == cmd::DrawRectangle::cmd_id && cmd::DrawRectangle::validate_size(payload_size)) {
                const auto &rectangle = reinterpret_cast<const cmd::DrawRectangle *>(payload_data)->rectangle;
                if (m8ec::Config::debug_m8_protocol) {
                    LOG("DrawRectangle: x:%u,y:%u,w:%u,h:%u\n", rectangle.pos.x, rectangle.pos.y, rectangle.size.w,
                        rectangle.size.h);
                }
                service->display.draw_rectangle(rectangle);
            }
            else if (cmd_id == cmd::PrintSystemInfo::cmd_id && cmd::PrintSystemInfo::validate_size(payload_size)) {
                const auto &system_info = reinterpret_cast<const cmd::PrintSystemInfo *>(payload_data)->system_info;
                const char *device_type[] = {"Headless", "M8 Beta", "M8 Production"};
                static bool system_info_already_printed = false;
                if (!system_info_already_printed) {
                    LOG("System Info: device type: %s, firmware version %d.%d.%d\n", device_type[system_info.hw_type],
                        system_info.version.major, system_info.version.minor, system_info.version.patch);
                    system_info_already_printed = true;
                }
                service->display.set_large_mode(system_info.font_mode == SystemInfo::FontMode::large);
            }
            else {
                if (m8ec::Config::debug_m8_protocol) {
                    LOG("Error: Unknown command: %02x of size %lu\n", data[0], payload_size);
                }
                return 0;
            }
            return 1;
        },
    };
    slip_handler_s slip;

    if (SLIP_NO_ERROR != slip_init(&slip, &slip_descriptor)) {
        LOG("Error: SLIP: slip_init failed\n");
        return;
    }

    bool first_run = true;
    while (true) {
        if (first_run || !periph::UsbCdc::get_instance().ready()) {
            while (!periph::UsbCdc::get_instance().ready()) {
                LOG("Waiting for USB virtual COM");
                fonas::delay_ms(250);
            }
            enable_display();
            reset_display();
            first_run = false;
        }
        std::uint8_t buffer[Config::usbcdc_to_slip_buffer_size];
        const auto bytes_read = m8ec::periph::UsbCdc::get_instance().read(buffer, sizeof(buffer));
        for (std::size_t i = 0; i < bytes_read; i++) {
            const slip_error_t n = slip_read_byte(&slip, buffer[i]);
            if (n != SLIP_NO_ERROR) {
                if (m8ec::Config::debug_slip) {
                    LOG("Error: SLIP: %d\n", n);
                }
                if (n == SLIP_ERROR_INVALID_PACKET) {
                    this->reset_display();
                }
            }
        }
    }
}

void print_keys_change(const Keys::State &prev_keys_state, const Keys::State &keys_state) {

    for (const auto &key : Keys::keys) {
        if (prev_keys_state.get(key) != keys_state.get(key)) {
            LOG("%s%c\n", Keys::key_to_string(key), (keys_state.get(key) ? '+' : '-'));
        }
    }
}

void Keys::Run() {
    while (true) {
        Keys::get_instance().init();
        Keys::State prev_keys_state = {};
        while (true) {
            const Keys::State keys_state = this->ll_get_state();
            if (prev_keys_state.underlying != keys_state.underlying) {
                this->protocol_service.send_keys_state(keys_state);
                if (m8ec::Config::debug_keys) {
                    print_keys_change(prev_keys_state, keys_state);
                }
                prev_keys_state = keys_state;
            }
            Thread::DelayUntil(m8ec::Config::keys_refresh_period);
        }
    }
}

} // namespace m8ec::m8::protocol
