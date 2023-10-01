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

extern "C" void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart == &huart1) {
        m8ec::periph::Uart1::get_instance().ll_async_write_completed_cb_from_isr();
    }
}

extern "C" void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart == &huart1) {
        m8ec::periph::Uart1::get_instance().ll_async_read_completed_cb_from_isr();
    }
}
