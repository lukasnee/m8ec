/*
 * m8ec - Embedded Client for the Dirtywave M8 Headless device.
 * Copyright (C) 2023 Lukas Neverauskis https://github.com/lukasnee
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "m8ec/periph/Spi1.hpp"

#include "stm32xxxx_hal.h"

extern SPI_HandleTypeDef hspi1;

extern "C" void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
    if (hspi == &hspi1) {
        m8ec::periph::Spi1::get_instance().ll_async_write_completed_cb();
    }
}

extern "C" void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi) {
    if (hspi == &hspi1) {
        m8ec::periph::Spi1::get_instance().ll_async_read_completed_cb();
    }
}

extern "C" void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {
    if (hspi == &hspi1) {
        m8ec::periph::Spi1::get_instance().ll_async_read_write_completed_cb();
    }
}

extern "C" void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi) {
    if (hspi == &hspi1) {
        m8ec::periph::Spi1::get_instance().ll_async_abnormal_cb();
    }
}

extern "C" void HAL_SPI_AbortCpltCallback(SPI_HandleTypeDef *hspi) {
    if (hspi == &hspi1) {
        m8ec::periph::Spi1::get_instance().ll_async_abnormal_cb();
    }
}

extern "C" void HAL_SPI_SuspendCallback(SPI_HandleTypeDef *hspi) {
    if (hspi == &hspi1) {
        m8ec::periph::Spi1::get_instance().ll_async_abnormal_cb();
    }
}
