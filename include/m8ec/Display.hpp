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

#include "lvgl.h"

namespace m8ec {

// The hardware-bound display singleton.
struct Display {

    static Display &get_instance();

    bool init();

    lv_display_t *lcd();

private:
    lv_display_t *lv_display = nullptr;

    Display() = default;
};

} // namespace m8ec
