/*
 * fonas - FreeRTOS C++ addons.
 * Copyright (C) 2023 Lukas Neverauskis https://github.com/lukasnee
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "fonas/EventDrivenReader.hpp"

namespace fonas {

bool EventDrivenReader::init() { return this->ll_init(); }

bool EventDrivenReader::read(std::uint8_t *data, std::size_t size, TickType_t timeout_ticks) {
    LockGuard lock_guard(this->read_mutex);
    // assure that the binary semaphore is not already given from previous timed out read() call.
    this->read_semaphore.Give();
    this->read_semaphore.Take();
    if (!this->ll_async_read(data, size)) {
        return false;
    }
    if (this->read_semaphore.Take(timeout_ticks) != pdTRUE) {
        return false;
    }
    return true;
}

bool EventDrivenReader::deinit() { return this->ll_deinit(); }

void EventDrivenReader::ll_async_read_completed_cb_from_isr() {
    BaseType_t *pxHigherPriorityTaskWoken = nullptr;
    this->read_semaphore.GiveFromISR(pxHigherPriorityTaskWoken);
}

void EventDrivenReader::ll_async_read_completed_cb() { this->read_semaphore.Give(); }

} // namespace fonas