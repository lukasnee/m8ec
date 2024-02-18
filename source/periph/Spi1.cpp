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

#include <array>

extern SPI_HandleTypeDef hspi1;

DMA_BUFFER std::array<std::uint8_t, 2 * 1024> spi1_dma_tx_buffer;
DMA_BUFFER std::array<std::uint8_t, 1024> spi1_dma_rx_buffer;

namespace m8ec::periph {

Spi1 &Spi1::get_instance() {
    static Spi1 instance;
    return instance;
}

bool Spi1::ll_init() { return true; /* handled in main.c */ }

bool Spi1::ll_async_read(std::uint8_t *data, std::size_t size) {
    if (!data) {
        return false;
    }
    if (size > spi1_dma_rx_buffer.size()) {
        return false;
    }
    if (HAL_OK != HAL_SPI_Receive_DMA(&hspi1, spi1_dma_rx_buffer.data(), size)) {
        return false;
    }
    std::copy(spi1_dma_rx_buffer.begin(), spi1_dma_rx_buffer.begin() + size, data);
    return true;
}

bool Spi1::ll_async_write(const std::uint8_t *data, std::size_t size) {
    if (!data) {
        return false;
    }
    if (size > spi1_dma_tx_buffer.size()) {
        return false;
    }
    std::copy(data, data + size, spi1_dma_tx_buffer.begin());
    if (HAL_OK != HAL_SPI_Transmit_DMA(&hspi1, spi1_dma_tx_buffer.data(), size)) {
        return false;
    }
    return true;
}

bool Spi1::ll_async_read_write(std::uint8_t *rd_data, const std::uint8_t *wr_data, std::size_t size) {
    if (!rd_data || !wr_data) {
        return false;
    }
    if (size > spi1_dma_tx_buffer.size() || size > spi1_dma_rx_buffer.size()) {
        return false;
    }
    std::copy(wr_data, wr_data + size, spi1_dma_tx_buffer.begin());
    if (HAL_OK != HAL_SPI_TransmitReceive_DMA(&hspi1, spi1_dma_tx_buffer.data(), spi1_dma_rx_buffer.data(), size)) {
        return false;
    }
    std::copy(spi1_dma_rx_buffer.begin(), spi1_dma_rx_buffer.begin() + size, rd_data);
    return true;
}

bool Spi1::ll_deinit() { return true; /* handled in main.c */ }

} // namespace m8ec::periph
