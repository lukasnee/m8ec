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

#include "m8ec/drivers/DisplayILI9341.hpp"
#include "m8ec/m8/protocol.hpp"

#include "ln/logger/logger.hpp"

#include <cstddef>

namespace m8ec {

struct M8Display : public m8::protocol::Service::Display {

    M8Display(m8ec::drivers::DisplayILI9341 &displayILI9341) : displayILI9341(displayILI9341) {}

    using Character = m8::protocol::Character;
    using Rectangle = m8::protocol::Rectangle;
    using Waveform = m8::protocol::Waveform;

    void draw_waveform(const Waveform &waveform, uint16_t waveform_width);
    void draw_rectangle(const Rectangle &rectangle);
    int draw_character(const Character &character);
    void set_large_mode(int enabled);

private:
    ili9341_t *lcd() { return this->displayILI9341.get_ili9341_t(); }

    m8ec::drivers::DisplayILI9341 &displayILI9341;

    LOG_MODULE_CLASS_MEMBER("m8ec::M8Display", LOGGER_LEVEL_NOTSET);
};

} // namespace m8ec
