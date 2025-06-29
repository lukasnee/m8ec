/*
 * m8ec - Embedded Client for the Dirtywave M8 Headless device.
 * Copyright (C) 2023 Lukas Neverauskis https://github.com/lukasnee
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "m8ec/Display.hpp"

#include "ILI9341/ili9341.h"
#include "ILI9341/ili9341_gfx.h"
#include "m8ec/m8ec.h" // LOG
#include "m8ec/periph/Spi1.hpp"
#include "main.h"

extern SPI_HandleTypeDef hspi1; // main.c

namespace m8ec {

Display &Display::get_instance() {
    static Display instance;
    return instance;
}

bool Display::init() {
    if (this->ili9341) {
        logger.error("Display::init: already initialized");
        return false;
    }
    this->ili9341 = ili9341_new(
        &hspi1,
        [](uint8_t *data, uint16_t size) -> uint16_t {
            return m8ec::periph::Spi1::get_instance().write(data, size) ? size : 0;
        },
        [](uint8_t *rd_data, const uint8_t *wr_data, uint16_t size) -> uint16_t {
            return m8ec::periph::Spi1::get_instance().read_write(rd_data, wr_data, size) ? size : 0;
        },
        [](uint32_t delay_ms) { vTaskDelay(pdMS_TO_TICKS(delay_ms)); }, TFT_RESET_GPIO_Port, TFT_RESET_Pin,
        TFT_CS_GPIO_Port, TFT_CS_Pin, TFT_DC_GPIO_Port, TFT_DC_Pin, isoLandscape, NULL, 0, NULL, 0, itsNONE, itnNONE);
    if (!this->ili9341) {
        logger.error("ili9341_new failed");
        return false;
    }
    ili9341_fill_screen(this->ili9341, this->get_bg_color());
    return true;
}

ili9341_t *Display::lcd() { return this->ili9341; }

} // namespace m8ec
