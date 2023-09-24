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

#define FONAS_PANIC() // TODO

#include "thread.hpp"
#include "ticks.hpp"
#include "semaphore.hpp"
#include "queue.hpp"
#include "mutex.hpp"

namespace fonas {

using cpp_freertos::Thread;
using cpp_freertos::BinarySemaphore;
using cpp_freertos::Queue;
using cpp_freertos::MutexStandard;
using cpp_freertos::MutexRecursive;
using cpp_freertos::LockGuard;

void delay_ms(std::size_t ms);
std::size_t get_uptime_ticks();
std::size_t get_uptime_ms();

} // namespace fonas