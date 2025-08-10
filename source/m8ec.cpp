/*
 * m8ec - Embedded Client for the Dirtywave M8 Headless device.
 * Copyright (C) 2023 Lukas Neverauskis https://github.com/lukasnee
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "platform.h"

#include "m8ec/drivers.hpp"
#include "m8ec/services.hpp"

#include "ln/ln.hpp"
#include "ln/logger/logger.hpp"

#include <cstdio>

LOG_MODULE(m8ec, LOGGER_LEVEL_INFO);

namespace m8ec {


static bool init_sw_periphs() {
    LN_ASSERT(Display::get_instance().init());
    LOG_INFO("Display OK");
    return true;
}


void launch() {
    LN_ASSERT(drivers::init());
    LN_ASSERT(init_sw_periphs());
    LN_ASSERT(services::init());
}

} // namespace m8ec

extern "C" void platform_app() { m8ec::launch(); }

extern "C" void platform_print_freertos_stats(const char *buff) {
    const ili9341_text_attr_t attr{.font = &ili9341_font_trash80_stealth57,
                                   .fg_color = ILI9341_WHITE,
                                   .bg_color = ILI9341_BLACK,
                                   .origin = {.x = 0, .y = 0},
                                   .h_wrap = ILI9341_H_WRAP_OFF};
    ili9341_draw_string(m8ec::Display::get_instance().lcd(), attr, buff);
}
