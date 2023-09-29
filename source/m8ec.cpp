/*
 * m8ec - Embedded Client for the Dirtywave M8 Headless device.
 * Copyright (C) 2023 Lukas Neverauskis https://github.com/lukasnee
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "m8ec/m8ec.h"
#include "m8ec/KeysThread.hpp"
#include "m8ec/m8ec.hpp"

#include "m8ec/periph/Uart1.hpp"
#include "m8ec/periph/UsbCdc.hpp"

#include "m8ec/display.hpp"
#include "m8ec/m8_protocol.hpp"

#include "fonas/fonas.hpp"

#include <cstdio>

namespace m8ec {

static bool init_periphs() {
    if (!periph::Uart1::get_instance().init()) {
        printf("error: periph::Uart1::get_instance().init failed\n");
        FONAS_PANIC();
        return false;
    }
    if (!periph::UsbCdc::get_instance().init()) {
        printf("error: periph::UsbCdc::get_instance().init failed\n");
        FONAS_PANIC();
        return false;
    }
    return true;
}

void launch() {
    if (!init_periphs()) {
        printf("error: init_periphs failed\n");
        FONAS_PANIC();
        return;
    }
    if (!display::initialize()) {
        printf("error: display::initialize failed\n");
        FONAS_PANIC();
        return;
    }
    while (!periph::UsbCdc::get_instance().ready()) {
        display::draw_string("waiting for USB virtual COM\n");
        printf("waiting for USB virtual COM\n");
        fonas::delay_ms(100);
    }
    if (!KeysThread::get_instance().init()) {
        printf("error: KeysThread::get_instance().init failed\n");
        FONAS_PANIC();
        return;
    }
    if (!m8_protocol::init()) {
        printf("error: m8_protocol::init\n");
        FONAS_PANIC();
        return;
    }
}

} // namespace m8ec

extern "C" void m8ec_launch() { m8ec::launch(); }
