/*
 * m8ec - Embedded Client for the Dirtywave M8 Headless device.
 * Copyright (C) 2023 Lukas Neverauskis https://github.com/lukasnee
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "platform.h"

#include "m8ec/m8ec.hpp"

#if defined(STM32H750xx)
#include "m8ec/periph/Uart4.hpp"
#elif defined(STM32F411xE)
#include "m8ec/periph/Uart1.hpp"
#endif
#include "m8ec/periph/UsbCdc.hpp"

#include "LuaThread.hpp"
#include "m8ec/Display.hpp"
#include "m8ec/M8Display.hpp"
#include "m8ec/m8/protocol.hpp"

#include "fonas/fonas.hpp"

#include <cstdio>

namespace m8ec {

static bool init_hw_periphs() {
#if defined(STM32H750xx)
    if (!periph::Uart4::get_instance().init()) {
        LOG("error: periph::Uart4::get_instance().init failed\n");
        FONAS_PANIC();
        return false;
    }
    LOGD("UART4 OK\n");
#elif defined(STM32F411xE)
    if (!periph::Uart1::get_instance().init()) {
        LOG("error: periph::Uart1::get_instance().init failed\n");
        FONAS_PANIC();
        return false;
    }
    LOGD("UART1 OK\n");
#endif

    if (!periph::UsbCdc::get_instance().init()) {
        LOG("error: periph::UsbCdc::get_instance().init failed\n");
        FONAS_PANIC();
        return false;
    }
    LOGD("USB CDC OK\n");

    return true;
}

M8Display m8_display(Display::get_instance());
namespace m8::protocol {
static Service service(m8_display);

::m8ec::m8::protocol::Keys & ::m8ec::m8::protocol::Keys::get_instance() {
    static ::m8ec::m8::protocol::Keys instance("keysSvc", 2 * 1024, 1, service);
    return instance;
}
} // namespace m8::protocol

struct LivenessThread : fonas::Thread {

    static LivenessThread &get_instance() {
        static LivenessThread instance;
        return instance;
    }

private:
    LivenessThread() : fonas::Thread("liveness", 1024, 1) {}

    void Run() final {
        ili9341_text_attr_t attr{.font = &ili9341_font_trash80_stealth57,
                                 .fg_color = ILI9341_WHITE,
                                 .bg_color = ILI9341_BLACK,
                                 .origin = {.x = 0, .y = 0},
                                 .h_wrap = ILI9341_H_WRAP_OFF};
        while (true) {
            static const char loadingChars[] = {'|', '/', '-', '\\'};
            static uint8_t loadingCharIndex = 0;
            ili9341_draw_char(Display::get_instance().lcd(), attr, loadingChars[loadingCharIndex]);
            loadingCharIndex = (loadingCharIndex + 1) % sizeof(loadingChars);
            fonas::delay_ms(250);
        }
    }
};

static bool init_apps() {
    if (!Display::get_instance().init()) {
        LOG("error: display::initialize failed\n");
        FONAS_PANIC();
        return false;
    }
    LOGD("Display OK\n");

    LivenessThread::get_instance().Start();
    LuaThread::get_instance().Start();

    // TODO: figure out why Keys::Service makes system hang
    // if (!keysService.init()) {
    //     LOG("error: keysService.init failed\n");
    //     FONAS_PANIC();
    //     return false;
    // }
    // LOGD("Keys::Service OK\n");

    if (!m8::protocol::service.init()) {
        LOG("error: m8::protocol::init\n");
        FONAS_PANIC();
        return false;
    }
    LOGD("m8::protocol OK\n");

    return true;
}

void launch() {
    if (!init_hw_periphs()) {
        LOG("error: init_hw_periphs failed\n");
        FONAS_PANIC();
        return;
    }
    if (!init_apps()) {
        LOG("error: init_apps failed\n");
        FONAS_PANIC();
        return;
    }
}

} // namespace m8ec

extern "C" void platform_app() { m8ec::launch(); }
