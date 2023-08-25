/*
 * m8ec - Embedded Client for the Dirtywave M8 Headless device
 * Copyright (C) 2023 Lukas Neverauskis https://github.com/lukasnee
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "m8ec/m8ec.h"
#include "m8ec/m8ec.hpp"

#include "ILI9341/ili9341_gfx.h"

#include "usb_host.h" // TODO decouple

#include "fibsys/fibsys.hpp"

#include <cstdint>

#define FIBSYS_PANIC() // TODO

namespace m8ec {

class Main : public fibsys::Thread {
public:
    using Thread::Thread;

    void init(ili9341_t *screen) {
        this->screen = screen;
        if (!this->screen) {
            FIBSYS_PANIC();
            return;
        }
        this->Start();
    }

    void write(const std::uint8_t *data, std::size_t size) {
        if (!this->screen) {
            return;
        }
        // ili9341_draw_string(this->screen, textAttr, "m8ec::launch()");
        for (std::size_t i = 0; i < size; ++i) {
            if (data[i] == '\n') {
                this->textAttr.origin_x = 0;
                this->textAttr.origin_y += 10;
                continue;
            }
            ili9341_draw_char(this->screen, this->textAttr, data[i]);
            this->textAttr.origin_x += 7;
        }
    }

private:
    void Run() final override {
        ili9341_fill_screen(this->screen, ILI9341_BLACK);
        this->textAttr = {
            .font = &ili9341_font_7x10,
            .fg_color = ILI9341_WHITE,
            .bg_color = ILI9341_BLACK,
            .origin_x = 0,
            .origin_y = 0,
        };
        while (true) {
            Thread::Delay(200);
        }
    }

    ili9341_text_attr_t textAttr;
    ili9341_t *screen = nullptr;
};

static Main main("main", 1024, 1);

} // namespace m8ec

extern "C" void m8ec_launch(ili9341_t *screen) { m8ec::main.init(screen); }

#include <errno.h>
#include <sys/unistd.h>

/**
 * @brief Standard output redirection to the screen
 */
extern "C" int _write(int fd, char *ptr, int len) {
    if ((fd != STDOUT_FILENO) && (fd != STDERR_FILENO)) {
        errno = EBADF;
        return -1;
    }
    m8ec::main.write(reinterpret_cast<std::uint8_t *>(ptr), len);
    return len;
}
