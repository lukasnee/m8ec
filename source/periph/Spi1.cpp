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

bool Spi1::ll_async_read(std::uint8_t *data, std::size_t size) {
    if (HAL_OK != HAL_SPI_Receive_DMA(&hspi1, data, size)) {
        return false;
    }
    return true;
}

bool Spi1::ll_async_write(const std::uint8_t *data, std::size_t size) {
    if (HAL_OK != HAL_SPI_Transmit_DMA(&hspi1, const_cast<std::uint8_t *>(data), size)) {
        return false;
    }
    return true;
}

bool Spi1::ll_async_read_write(std::uint8_t *rd_data, const std::uint8_t *wr_data, std::size_t size) {
    if (HAL_OK != HAL_SPI_TransmitReceive_DMA(&hspi1, const_cast<std::uint8_t *>(wr_data), rd_data, size)) {
        return false;
    }
    return true;
}

bool Spi1::ll_deinit() { return true; /* handled in main.c */ }

} // namespace m8ec::periph
