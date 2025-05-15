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

#pragma once

#include "fonas/fonas.hpp"
#include "fonas/logger/logger.hpp"

#include <cstdint>

namespace m8ec::m8::protocol {

#pragma pack(push, 1)
struct Position {
    uint16_t x;
    uint16_t y;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct Size {
    uint16_t w;
    uint16_t h;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct Rectangle {
    Position pos;
    Size size;
    Color color;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct Character {
    char c;
    Position pos;
    Color foreground;
    Color background;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct Waveform {
    Color color;
    uint8_t buffer[320];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct SystemInfo {
    enum HwType : uint8_t {
        m8_headless = 0x00,
        m8_beta = 0x01,
        m8_production = 0x02,
    };
    struct Version {
        uint8_t major;
        uint8_t minor;
        uint8_t patch;
    };
    enum FontMode : uint8_t {
        small = 0x00,
        large = 0x01,
    };
    HwType hw_type;
    Version version;
    FontMode font_mode;
};
#pragma pack(pop)

namespace Keys {

enum Key : uint8_t {
    edit = 0,
    option,
    right,
    play,
    shift,
    down,
    up,
    left,

    none = 0xff,
};

const char *key_to_string(Key key);

struct State {
    void set(Key key, bool value) {
        if (value) {
            this->underlying |= 1 << key;
        }
        else {
            this->underlying &= ~(1 << key);
        }
    }

    bool get(Key key) const { return this->underlying & (1 << key); }

    std::uint8_t underlying;
};

} // namespace Keys

struct Service : public fonas::Thread {

    struct Display {
        virtual void draw_waveform(const Waveform &waveform, uint16_t waveform_width) = 0;
        virtual void draw_rectangle(const Rectangle &rectangle) = 0;
        virtual int draw_character(const Character &character) = 0;
        virtual void set_large_mode(int enabled) = 0;
        // virtual void view_changed(int view) = 0; // TODO
    };

    Service(Display &display);

    bool init();

    void enable_display();
    void reset_display();
    void send_keys_state(Keys::State keys_state);

private:
    void Run() final;

    Display &display;

    fonas::Logger::Module logger{"m8ec::m8::protocol::Service"};
};
namespace Keys {

struct Svc : fonas::Thread {
    static constexpr Key keys[] = {Key::edit,  Key::option, Key::right, Key::play,
                                   Key::shift, Key::down,   Key::up,    Key::left};
    static Svc &get_instance();

protected:
    bool ll_init();
    State ll_get_state();

private:
    ~Svc() = default;
    Svc(const Svc &) = delete;
    Svc &operator=(const Svc &) = delete;
    Svc(const char *Name, uint16_t StackDepth, UBaseType_t Priority, m8ec::m8::protocol::Service &protocol_service)
        : fonas::Thread(Name, StackDepth, Priority), protocol_service(protocol_service) {}
    void Run() final override;

    void print_keys_change(const State &prev_keys_state, const State &keys_state);

    m8ec::m8::protocol::Service &protocol_service;

    fonas::Logger::Module logger{"m8ec::m8::protocol::Keys::Svc"};
};
} // namespace Keys
} // namespace m8ec::m8::protocol
