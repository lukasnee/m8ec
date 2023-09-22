/*
 * m8ec - Embedded Client for the Dirtywave M8 Headless device.
 * Copyright (C) 2023 Lukas Neverauskis https://github.com/lukasnee
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "m8ec/periph/Uart1.hpp"

#include "stm32f4xx_hal.h"

extern UART_HandleTypeDef huart1;

namespace m8ec::periph {

Uart1 &Uart1::get_instance() {
    static Uart1 instance;
    return instance;
}

bool Uart1::ll_init() { return true; /* handled in main.c */ }

bool Uart1::ll_async_write(const std::uint8_t *data, std::size_t size) {
    if (HAL_OK != HAL_UART_Transmit_DMA(&huart1, const_cast<std::uint8_t *>(data), size)) {
        return false;
    }
    return true;
}

bool Uart1::ll_async_read(std::uint8_t *pData, std::size_t size) {
    if (HAL_OK != HAL_UART_Receive_DMA(&huart1, pData, size)) {
        return false;
    }
    return true;
}

bool Uart1::ll_deinit() { return true; /* handled in main.c */ }

} // namespace m8ec::periph
