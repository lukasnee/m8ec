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

#include <cstdint>
namespace m8ec {

struct Config {
    static constexpr bool debug_m8_protocol = false;
    static constexpr bool debug_slip = false;
    static constexpr bool debug_keys = false;

    static constexpr std::uint32_t keys_refresh_period = 20;
    static constexpr std::uint32_t usb_cdc_rx_stream_buffer_size = 1024; // may be overkill // TODO fine tune tune
    static constexpr std::uint32_t usbcdc_to_slip_buffer_size = 1024; // may be overkill // TODO fine tune
    static constexpr std::uint32_t slip_buffer_size = 1024; // may be overkill // TODO fine tune
};

} // namespace m8ec
