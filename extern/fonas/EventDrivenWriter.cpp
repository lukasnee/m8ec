/*
 * fonas - FreeRTOS C++ addons.
 * Copyright (C) 2023 Lukas Neverauskis https://github.com/lukasnee
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "fonas/EventDrivenReaderWriter.hpp"

namespace fonas {

bool EventDrivenReaderWriter::init() { return this->ll_init(); }

bool EventDrivenReaderWriter::write(const std::uint8_t *data, std::size_t size, TickType_t timeout_ticks) {
    LockGuard lock_guard(this->write_mutex);
    // assure that the binary semaphore is not already given from previous timed out write() call.
    this->write_semaphore.Give();
    this->write_semaphore.Take();
    if (!this->ll_async_write(data, size)) {
        return false;
    }
    if (this->write_semaphore.Take(timeout_ticks) != pdTRUE) {
        return false;
    }
    return true;
}

bool EventDrivenReaderWriter::deinit() { return this->ll_deinit(); }

void EventDrivenReaderWriter::ll_async_write_completed_cb_from_isr() {
    BaseType_t *pxHigherPriorityTaskWoken = nullptr;
    this->write_semaphore.GiveFromISR(pxHigherPriorityTaskWoken);
}

void EventDrivenReaderWriter::ll_async_write_completed_cb() { this->write_semaphore.Give(); }

} // namespace fonas