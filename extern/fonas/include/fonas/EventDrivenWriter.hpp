/*
 * fonas - FreeRTOS C++ addons.
 * Copyright (C) 2023 Lukas Neverauskis https://github.com/lukasnee
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#pragma once

#include "fonas/fonas.hpp"

#include <cstdint>

namespace fonas {

class EventDrivenWriter {
public:
    /**
     * @brief Initialize.
     *
     * @return true success.
     * @return false failure.
     */
    bool init();

    /**
     * @brief Write synchronously.
     *
     * @param data
     * @param size
     * @param timeout_ticks
     * @return true success.
     * @return false failure.
     */
    bool write(const std::uint8_t *data, std::size_t size, TickType_t timeout_ticks = portMAX_DELAY);

    /**
     * @brief Deinitialize.
     *
     * @return true success.d
     * @return false failure.
     */
    bool deinit();

    /**
     * @brief Low-level interrupt callback signaling write completion. Alternatively use ll_async_write_completed_cb() in thread
     * context.
     */
    void ll_async_write_completed_cb_from_isr();

    /**
     * @brief Low-level thread callback signaling write completion. Alternatively use ll_async_write_completed_cb_from_isr() in
     * interrupt context.
     */
    void ll_async_write_completed_cb();

protected:
    /**
     * @brief Low-level initialization.
     *
     * @return true success.
     * @return false failure.
     */
    virtual bool ll_init() = 0;

    /**
     * @brief Low-level asynchronous write.
     *
     * @param data
     * @param size
     * @return true success.
     * @return false failure.
     */
    virtual bool ll_async_write(const std::uint8_t *data, std::size_t size) = 0;

    /**
     * @brief Low-level deinitialization.
     *
     * @return true success.
     * @return false failure.
     */
    virtual bool ll_deinit() = 0;

private:
    MutexStandard mutex;
    BinarySemaphore semaphore;
};

} // namespace fonas
