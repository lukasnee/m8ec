/*
 * m8ec - Embedded Client for the Dirtywave M8 Headless device.
 * Copyright (C) 2023 Lukas Neverauskis https://github.com/lukasnee
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

// Copyright 2021 Jonne Kokkonen
// Released under the MIT licence, https://opensource.org/licenses/MIT

#include "m8ec/display.hpp"
#include "m8ec/font.hpp"
#include "m8ec/m8_protocol.hpp"
#include "m8ec/periph/Spi1.hpp"

#include "ILI9341/ili9341_gfx.h"
#include "main.h"

#include <algorithm>
#include <cstdio>

extern SPI_HandleTypeDef hspi1; // main.c

static ili9341_t *ili9341_lcd = nullptr;

namespace m8ec::display {

bool initialize() {
    ili9341_lcd = ili9341_new(
        &hspi1,
        [](uint8_t *data, uint16_t size) -> uint16_t { return m8ec::periph::Spi1::get_instance().write(data, size) ? 1 : 0; },
        [](uint8_t *rd_data, const uint8_t *wr_data, uint16_t size) -> uint16_t {
            return m8ec::periph::Spi1::get_instance().read_write(rd_data, wr_data, size) ? 1 : 0;
        },
        TFT_RESET_GPIO_Port, TFT_RESET_Pin, TFT_CS_GPIO_Port, TFT_CS_Pin, TFT_DC_GPIO_Port, TFT_DC_Pin, isoLandscape, NULL, 0,
        NULL, 0, itsNONE, itnNONE);
    if (!ili9341_lcd) {
        printf("error: ili9341_new failed\n");
        return false;
    }
    ili9341_fill_screen(ili9341_lcd, ILI9341_BLACK);
    return true;
}

static void change_font(struct inline_font *font) {}

void set_large_mode(int enabled) { printf("error: set_large_mode: %d: not implemented\n", enabled); }

int draw_character(const m8_protocol::Character &character) {
    if (!ili9341_lcd) {
        return -1;
    }
    ili9341_text_attr_t textAttr;
    textAttr.bg_color = __ILI9341_COLOR565(character.background.r, character.background.g, character.background.b);
    textAttr.fg_color = __ILI9341_COLOR565(character.foreground.r, character.foreground.g, character.foreground.b);
    textAttr.font = &font::trash80_stealth57;
    textAttr.origin_x = character.pos.x;
    constexpr decltype(textAttr.origin_y) char_to_rect_y_adjustment = 3U;
    textAttr.origin_y = character.pos.y + char_to_rect_y_adjustment;
    const auto c = static_cast<char>(character.c);
    ili9341_draw_char(ili9341_lcd, textAttr, c);
    return character.c;
}

void draw_rectangle(const m8_protocol::Rectangle &rectangle) {
    if (!ili9341_lcd) {
        return;
    }
    ili9341_color_t color = __ILI9341_COLOR565(rectangle.color.r, rectangle.color.g, rectangle.color.b);
    ili9341_fill_rect(ili9341_lcd, color, rectangle.pos.x, rectangle.pos.y, rectangle.size.w, rectangle.size.h);
}

void draw_waveform(const m8_protocol::Waveform &waveform, size_t waveform_size) {
    if (!ili9341_lcd) {
        return;
    }
    if (waveform_size == 0) {
        return;
    }
    const ili9341_color_t bg_color = ILI9341_BLACK;
    struct Canvas {
        uint16_t x;
        uint16_t y;
        uint16_t w;
        uint16_t h;
    };
    static m8_protocol::Waveform waveform_prev = waveform;
    const Canvas canvas = Canvas{0, 0, 320 - waveform_size, 21};
    ili9341_color_t fg_color = __ILI9341_COLOR565(waveform.color.r, waveform.color.g, waveform.color.b);
    for (int i = 0; i < waveform_size; i++) {
        // Limit value because the oscilloscope commands seem to glitch occasionally
        const auto waveform_prev_value = std::min(waveform_prev.buffer[i], static_cast<uint8_t>(20));
        const auto waveform_value = std::min(waveform.buffer[i], static_cast<uint8_t>(20));
        ili9341_draw_pixel(ili9341_lcd, bg_color, canvas.x + i, waveform_prev_value);
        ili9341_draw_pixel(ili9341_lcd, fg_color, canvas.x + i, waveform_value);
    }
    waveform_prev = waveform;
}

void draw_string(const char *str) {
    if (!ili9341_lcd) {
        return;
    }
    ili9341_text_attr_t textAttr;
    textAttr.bg_color = ILI9341_BLACK;
    textAttr.fg_color = ILI9341_WHITE;
    textAttr.font = &ili9341_font_7x10;
    textAttr.origin_x = 0;
    textAttr.origin_y = 0;
    ili9341_draw_string(ili9341_lcd, textAttr, const_cast<char *>(str));
}
} // namespace m8ec::display
