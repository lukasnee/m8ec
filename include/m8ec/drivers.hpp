/*
 * m8ec - Embedded Client for the Dirtywave M8 Headless device.
 * Copyright (C) 2023 Lukas Neverauskis https://github.com/lukasnee
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#pragma once

#include "m8ec/m8ec.h"
#include "m8ec/config.hpp"

#include "m8ec/M8Display.hpp"
#include "m8ec/m8/protocol.hpp"
#include "m8ec/periph/UsbCdc.hpp"

namespace m8ec::drivers {

periph::UsbCdc &get_usb_cdc();
M8Display &get_display();

bool init();

} // namespace m8ec::drivers
