/*
 * m8ec - Embedded Client for the Dirtywave M8 Headless device
 * Copyright (C) 2023 Lukas Neverauskis https://github.com/lukasnee
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "ILI9341/ili9341.h"

    void m8ec_launch(ili9341_t *screen);

    void m8ec_serial_write_callback();
    void m8ec_serial_read_callback();

#ifdef __cplusplus
}
#endif
