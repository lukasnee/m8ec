/*
 * m8ec - Embedded Client for the Dirtywave M8 Headless device
 * Copyright (C) 2023 Lukas Neverauskis https://github.com/lukasnee
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

// Copyright 2021 Jonne Kokkonen
// Released under the MIT licence, https://opensource.org/licenses/MIT

#include "m8ec/KeysThread.hpp"
#include "m8ec/periph/UsbCdc.hpp"

namespace m8ec::m8_protocol {
void send_keys_state(IKeys::State keys_state) {
    uint8_t buf[2] = {'C', keys_state.underlying};
    if (!m8ec::periph::UsbCdc::get_instance().write(buf, 2)) {
        printf("m8_protocol::send_keys_state failed\n");
    }
}


} // namespace m8ec::m8_protocol
