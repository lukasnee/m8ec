/*
 * m8ec - Embedded Client for the Dirtywave M8 Headless device
 * Copyright (C) 2023 Lukas Neverauskis https://github.com/lukasnee
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "fibsys/fibsys.hpp"

namespace fibsys {

void delay_ms(std::size_t ms) {
    // cannot use cpp_freertos::Thread::Delay(ms); because it's not static method of Thread for some reason
    vTaskDelay(ms);
}
std::size_t get_uptime_ticks() { return static_cast<std::size_t>(cpp_freertos::Ticks::GetTicks()); }

std::size_t get_uptime_ms() {
    return static_cast<std::size_t>(cpp_freertos::Ticks::TicksToMs(cpp_freertos::Ticks::GetTicks()));
}
} // namespace fibsys
