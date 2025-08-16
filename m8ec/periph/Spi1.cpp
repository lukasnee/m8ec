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

namespace m8ec::periph {

Spi1 &Spi1::get_instance() {
    static Spi1 instance;
    return instance;
}

bool Spi1::ll_init() { return true; /* handled in main.c */ }

bool Spi1::ll_read_async(std::uint8_t *data, std::size_t size) {
    return HAL_OK == HAL_SPI_Receive_DMA(&hspi1, data, size);
}

bool Spi1::ll_write_async(const std::uint8_t *data, std::size_t size) {
    return HAL_OK == HAL_SPI_Transmit_DMA(&hspi1, data, size);
}

bool Spi1::ll_read_write_async(std::uint8_t *rd_data, const std::uint8_t *wr_data, std::size_t size) {
    return HAL_OK == HAL_SPI_TransmitReceive_DMA(&hspi1, wr_data, rd_data, size);
}

bool Spi1::ll_busy_writing() {
    return HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY || HAL_DMA_GetState(hspi1.hdmatx) != HAL_DMA_STATE_READY;
}

bool Spi1::ll_busy_reading() {
    return HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY || HAL_DMA_GetState(hspi1.hdmarx) != HAL_DMA_STATE_READY;
}

bool Spi1::ll_deinit() { return true; /* handled in main.c */ }

} // namespace m8ec::periph
