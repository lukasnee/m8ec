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

#include "m8ec/m8_protocol.hpp"
#include "m8ec/KeysThread.hpp"
#include "m8ec/display.hpp"
#include "m8ec/m8ec.hpp"
#include "m8ec/periph/UsbCdc.hpp"
#include "m8ec/slip.h"

#include "usb_host.h" // TODO decouple

#include <cstdio>
#include <cstring>

namespace m8ec::m8_protocol {

bool handle_cmd(uint8_t *data, uint32_t size);

static uint8_t slip_buffer[Config::slip_buffer_size];
static const slip_descriptor_s slip_descriptor = {
    .buf = slip_buffer,
    .buf_size = sizeof(slip_buffer),
    .recv_message = [](uint8_t *data, uint32_t size) -> int { return handle_cmd(data, size) ? 1 : 0; },
};
static slip_handler_s slip;

namespace cmd {

const char *cmd_id_to_name(uint8_t cmd_id);

template <typename Tcmd, uint8_t Tcmd_id, size_t Tcmd_size_min, size_t Tcmd_size_max> struct Cmd {
public:
    static constexpr uint8_t cmd_id = Tcmd_id;
    static constexpr size_t cmd_size_min = Tcmd_size_min;
    static constexpr size_t cmd_size_max = Tcmd_size_max;
    static bool validate_size(uint32_t actual_size) {
        if (actual_size < cmd_size_min || actual_size > cmd_size_max) {
            if (m8ec::Config::debug_m8_protocol) {
                printf(err_fmt_invalid_packet_length_range, cmd_id_to_name(cmd_id), cmd_size_min, cmd_size_max, actual_size);
            }
            return false;
        }
        return true;
    }

private:
    static constexpr const char *err_fmt_invalid_packet_length_range =
        "Error: %s: Invalid packet length: expected [%u %u], got %u\n";
};
#pragma pack(push, 1)
struct DrawRectangle : public Cmd<DrawRectangle, 0xFE, sizeof(Rectangle), sizeof(Rectangle)> {
    union {
        Rectangle rectangle;
    };
};
struct DrawCharacter : public Cmd<DrawCharacter, 0xFD, sizeof(Character), sizeof(Character)> {
    union {
        Character character;
    };
};
struct DrawWaveform
    : public Cmd<DrawWaveform, 0xFC, sizeof(Waveform::color), sizeof(Waveform::color) + sizeof(Waveform::buffer)> {
    union {
        Waveform waveform;
    };
};
struct PrintSystemInfo : public Cmd<SystemInfo, 0xFF, sizeof(SystemInfo), sizeof(SystemInfo)> {
    union {
        SystemInfo system_info;
    };
};
#pragma pack(pop)

const char *cmd_id_to_name(uint8_t cmd_id) {
    switch (cmd_id) {
    case DrawRectangle::cmd_id:
        return "DrawRectangle";
    case DrawCharacter::cmd_id:
        return "DrawCharacter";
    case DrawWaveform::cmd_id:
        return "DrawWaveform";
    case PrintSystemInfo::cmd_id:
        return "SystemInfo";
    default:
        return "Unknown";
    }
}

} // namespace cmd

class ServiceThread : public fonas::Thread {
public:
    using Thread::Thread;

    static ServiceThread &get_instance() {
        static ServiceThread instance("m8svc", 3 * 1024, 1);
        return instance;
    }

    bool init() { return this->Start(); }

private:
    void Run() final override {
        bool first_run = true;
        while (true) {
            if (first_run || !periph::UsbCdc::get_instance().ready()) {
                while (!periph::UsbCdc::get_instance().ready()) {
                    display::draw_string("waiting for USB virtual COM\n");
                    printf("waiting for USB virtual COM\n");
                    fonas::delay_ms(100);
                }
                enable_display();
                reset_display();
                first_run = false;
            }
            const auto byte = m8ec::periph::UsbCdc::get_instance().read();
            const slip_error_t n = slip_read_byte(&slip, byte);
            if (n != SLIP_NO_ERROR) {
                if (m8ec::Config::deubug_slip) {
                    printf("Error: SLIP: %d\n", n);
                }
                if (n == SLIP_ERROR_INVALID_PACKET) {
                    // m8_protocol::reset_display();
                }
            }
        }
    }
};

bool init() {
    if (SLIP_NO_ERROR != slip_init(&slip, &slip_descriptor)) {
        return false;
    }
    return ServiceThread::get_instance().init();
}

void enable_display() {
    uint8_t buf[] = {'E'};
    if (!m8ec::periph::UsbCdc::get_instance().write(buf, 1)) {
        printf("m8_protocol::enable_display failed\n");
    }
    printf("m8_protocol::enable_display\n");
}

void reset_display() {
    uint8_t buf[] = {'R'};
    if (!m8ec::periph::UsbCdc::get_instance().write(buf, 1)) {
        printf("m8_protocol::reset_display failed\n");
    }
    printf("m8_protocol::reset_display\n");
}

void send_keys_state(IKeys::State keys_state) {
    uint8_t buf[2] = {'C', keys_state.underlying};
    if (!m8ec::periph::UsbCdc::get_instance().write(buf, 2)) {
        printf("m8_protocol::send_keys_state failed\n");
    }
}

bool handle_cmd(uint8_t *data, uint32_t size) {
    if (size < 1) {
        return false;
    }
    const uint8_t cmd_id = data[0];
    const uint8_t *payload_data = &data[1];
    const uint32_t payload_size = size - 1;
    if (cmd_id == cmd::DrawRectangle::cmd_id) {
        if (!cmd::DrawRectangle::validate_size(payload_size)) {
            return false;
        }
        const auto &rectangle = reinterpret_cast<const cmd::DrawRectangle *>(payload_data)->rectangle;
        display::draw_rectangle(rectangle);
        return true;
    }
    else if (cmd_id == cmd::DrawCharacter::cmd_id) {
        if (!cmd::DrawCharacter::validate_size(payload_size)) {
            return false;
        }
        const auto &character = reinterpret_cast<const cmd::DrawCharacter *>(payload_data)->character;
        display::draw_character(character);
        return true;
    }
    else if (cmd_id == cmd::DrawWaveform::cmd_id) {
        if (!cmd::DrawWaveform::validate_size(payload_size)) {
            return false;
        }
        const auto &waveform = reinterpret_cast<const cmd::DrawWaveform *>(payload_data)->waveform;
        const auto waveform_size = payload_size - sizeof(Color);
        display::draw_waveform(waveform, waveform_size);
        return true;
    }
    else if (cmd_id == cmd::PrintSystemInfo::cmd_id) {
        if (!cmd::PrintSystemInfo::validate_size(payload_size)) {
            return false;
        }
        const auto &system_info = reinterpret_cast<const cmd::PrintSystemInfo *>(payload_data)->system_info;
        char *device_type[] = {"Headless", "M8 Beta", "M8 Production"};
        static bool system_info_already_printed = false;
        if (!system_info_already_printed) {
            printf("System Info: device type: %s, firmware version %d.%d.%d", device_type[system_info.hw_type],
                   system_info.version.major, system_info.version.minor, system_info.version.patch);
            system_info_already_printed = true;
        }
        display::set_large_mode(system_info.font_mode == SystemInfo::FontMode::large);
        return true;
    }
    if (m8ec::Config::debug_m8_protocol) {
        printf("Error: Unknown command: %02x of size %lu\n", data[0], payload_size);
    }
    return false;
}

} // namespace m8ec::m8_protocol
