/*
 * m8ec - Embedded Client for the Dirtywave M8 Headless device.
 * Copyright (C) 2023 Lukas Neverauskis https://github.com/lukasnee
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

// https://github.com/laamaa/m8c
// Copyright 2021 Jonne Kokkonen
// Released under the MIT licence, https://opensource.org/licenses/MIT

#include "m8ec/M8Display.hpp"

#include "m8ec/Display.hpp"
#include "m8ec/m8/protocol.hpp"

#include <array>

namespace m8ec {

void M8Display::set_large_mode(int enabled) { logger.error("set_large_mode: %d: not implemented", enabled); }

int M8Display::draw_character(const m8::protocol::Character &character) {
    if (!this->lcd()) {
        return -1;
    }
    ili9341_text_attr_t textAttr;
    textAttr.bg_color = __ILI9341_COLOR565(character.background.r, character.background.g, character.background.b);
    textAttr.fg_color = __ILI9341_COLOR565(character.foreground.r, character.foreground.g, character.foreground.b);
    textAttr.font = &ili9341_font_trash80_stealth57;
    textAttr.origin.x = character.pos.x;
    constexpr decltype(textAttr.origin.y) char_to_rect_y_adjustment = 3U;
    textAttr.origin.y = character.pos.y + char_to_rect_y_adjustment;
    const auto c = static_cast<char>(character.c);
    ili9341_draw_char(this->lcd(), textAttr, c);
    return character.c;
}

void M8Display::draw_rectangle(const m8::protocol::Rectangle &rectangle) {
    if (!this->lcd()) {
        return;
    }
    ili9341_color_t color = __ILI9341_COLOR565(rectangle.color.r, rectangle.color.g, rectangle.color.b);
    ili9341_fill_rect(this->lcd(), color, rectangle.pos.x, rectangle.pos.y, rectangle.size.w, rectangle.size.h);
    if (rectangle.size.h >= this->lcd()->screen_size.height || rectangle.size.w >= this->lcd()->screen_size.width) {
        this->display.set_bg_color(color); // remember the screen clear color
    }
}

struct Canvas {
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;
};

constexpr auto canvas_max = Canvas{0, 0, 320, 25};
std::array<uint8_t, (canvas_max.w * canvas_max.h / 8)> bmp_buff = {0};

void M8Display::draw_waveform(const m8::protocol::Waveform &waveform, uint16_t waveform_width) {
    if (!this->lcd()) {
        return;
    }
    if (waveform_width > canvas_max.w) {
        logger.warning("draw_waveform: waveform_width: %zu: too large", waveform_width);
        waveform_width = canvas_max.w; // limit the width
    }

    static uint16_t last_waveform_width = 0;
    static bool was_blank = false;

    const bool is_blank = 0 == waveform_width;
    if (is_blank && was_blank) {
        return; // skip
    }
    const auto canvas_w = is_blank ? last_waveform_width : waveform_width;
    if (canvas_w > canvas_max.w) {
        logger.warning("draw_waveform: canvas_w: %u: too large", canvas_w);
        return;
    }
    const auto canvas_x = static_cast<uint16_t>(canvas_max.w - canvas_w);
    const auto canvas = Canvas{canvas_x, canvas_max.y, canvas_w, canvas_max.h};
    last_waveform_width = waveform_width;
    bmp_buff.fill(0);
    for (std::size_t i = 0; i < waveform_width; i++) {
        // limit the waveform value (y) to canvas max height - allegedly it can glitch // TODO investigate
        const auto y = std::min(waveform.buffer[i], static_cast<uint8_t>(canvas.h - 1));
        const auto x = i;
        const auto bmp_index = (y * waveform_width) + x;
        const auto byte_index = bmp_index / 8;
        const auto byte_bit = bmp_index % 8;
        bmp_buff[byte_index] |= 1 << (7 - byte_bit);
    }
    const ili9341_color_t fg_color = __ILI9341_COLOR565(waveform.color.r, waveform.color.g, waveform.color.b);
    ili9341_draw_bitmap_1b(this->lcd(), fg_color, this->display.get_bg_color(), canvas.x, canvas.y, canvas.w, canvas.h,
                           bmp_buff.data());
    was_blank = is_blank;
}
} // namespace m8ec
