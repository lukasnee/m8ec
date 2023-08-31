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

#include "m8ec/command.hpp"
#include "m8ec/slip.h"

#include "ILI9341/ili9341_gfx.h"

#include "stm32f4xx_hal.h" // TODO decouple
#include "usb_host.h"      // TODO decouple
#include "usbh_cdc.h"      // TODO decouple

#include "fibsys/fibsys.hpp"

#include <cstdint>
#include <cstdio>

extern UART_HandleTypeDef huart1;     // TODO decouple
extern USBH_HandleTypeDef hUsbHostFS; // TODO decouple

#define FIBSYS_PANIC() // TODO

namespace helpers {
void hexdump(uint8_t *data, uint32_t length, uint32_t width = 16) {
    for (uint32_t i = 0; i < length; ++i) {
        if (i % width == 0) {
            printf("\n");
        }
        printf("%02x ", data[i]);
    }
    printf("\n");
}
} // namespace helpers
namespace m8ec {

struct Config {
    static constexpr uint32_t serial_read_size = 512;
};

static uint8_t slip_buffer[Config::serial_read_size];
static const slip_descriptor_s slip_descriptor = {
    .buf = slip_buffer,
    .buf_size = sizeof(slip_buffer),
    .recv_message = process_command,
};
static slip_handler_s slip;

fibsys::BinarySemaphore &serial_sem() {
    static fibsys::BinarySemaphore sem;
    return sem;
}

bool serial_write(uint8_t *data, uint32_t length, TickType_t timeout = 10);
bool serial_write(uint8_t *data, uint32_t length, TickType_t timeout) {
    const auto status = USBH_CDC_Transmit(&hUsbHostFS, data, length);
    if (USBH_OK != status) {
        printf("error: USBH_CDC_Transmit: %d\n", status);
        return false;
    }
    if (!serial_sem().Take(timeout)) {
        printf("writeSem.Take failed\n");
        return false;
    }
    return true;
}

bool serial_read(uint8_t *data, uint32_t length, TickType_t timeout = 10);
bool serial_read(uint8_t *data, uint32_t length, TickType_t timeout) {
    const auto status = USBH_CDC_Receive(&hUsbHostFS, data, length);
    if (USBH_OK != status) {
        printf("error: USBH_CDC_Receive: %d\n", status);
        return false;
    }
    if (!serial_sem().Take(timeout)) {
        printf("readSem.Take failed\n");
        return false;
    }
    return true;
}
void m8_enable_display() {
    uint8_t buf[] = {'E'};
    serial_write(buf, 1);
}

void m8_reset_display() {
    uint8_t buf[] = {'R'};
    serial_write(buf, 1);
}

bool m8_read(uint8_t *pbuff, uint32_t length) {
    if (!serial_read(pbuff, length)) {
        printf("serial_read failed\n");
        return false;
    }
    return true;
}

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
        HAL_UART_Transmit(&huart1, const_cast<std::uint8_t *>(data), size, 10);
        return;
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
        slip_init(&slip, &slip_descriptor);
        while (true) {
            Thread::Delay(1000);
            bool m8_already_init = false;
            if (!m8ec_virtual_com_ready()) {
                continue;
            }
            if (!m8_already_init) {
                Thread::Delay(10);
                m8_enable_display();
                Thread::Delay(10);
                m8_reset_display();
                m8_already_init = true;
            }

            while (1) {
                Thread::Delay(1000); // TODO tihs is to to reduce log spam, remove later
                uint8_t buff[Config::serial_read_size];
                if (!m8_read(buff, sizeof(buff))) {
                    continue;
                }
                // helpers::hexdump(buff, sizeof(buff));
                uint8_t *cur = buff;
                const uint8_t *end = buff + sizeof(buff);
                while (cur < end) {
                    int n = slip_read_byte(&slip, *(cur++));
                    if (n != SLIP_NO_ERROR) {
                        if (n == SLIP_ERROR_INVALID_PACKET) {
                            m8_reset_display();
                        }
                        else {
                            printf("Error: SLIP: %d\n", n);
                        }
                    }
                }
            }
            // render_screen(); // TODO
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

void m8ec_serial_write_callback() { m8ec::serial_sem().Give(); }
void m8ec_serial_read_callback() { m8ec::serial_sem().Give(); }
