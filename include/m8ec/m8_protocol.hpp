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

#include <cstdint>

#include "m8ec/platform/IKeys.hpp"

namespace m8ec::m8_protocol {

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

bool init();
void enable_display();
void reset_display();
void send_keys_state(IKeys::State keys_state);

} // namespace m8ec::m8_protocol
