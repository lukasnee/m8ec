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

#include "ILI9341/ili9341.h"
#include "ILI9341/ili9341_gfx.h"

#include "ln/logger/logger.hpp"

namespace m8ec {

// The hardware-bound display singleton.
struct Display {

    static Display &get_instance();

    bool init();

    ili9341_t *lcd();

    ili9341_color_t get_bg_color() { return this->bg_color; }
    void set_bg_color(ili9341_color_t color) { this->bg_color = color; }

private:
    Display() = default;

    ili9341_t *ili9341 = nullptr;

    ili9341_color_t bg_color = ILI9341_BLACK;

    LOG_MODULE_CLASS_MEMBER("m8ec::Display", LOGGER_LEVEL_DEBUG);
};

} // namespace m8ec
