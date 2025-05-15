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

#include "m8ec/Display.hpp"
#include "m8ec/M8Display.hpp"
#include "m8ec/m8/protocol.hpp"

#include "fonas/fonas.hpp"
#include "fonas/logger/logger.hpp"

#include <cstdio>

fonas::Logger::Module logger("m8ec");

namespace m8ec {
namespace m8 {
static M8Display display(Display::get_instance());
namespace protocol {
static m8::protocol::Service service(display);
} // namespace protocol
} // namespace m8

m8::protocol::Keys::Svc &m8::protocol::Keys::Svc::get_instance() {
    static Svc instance("keysSvc", 2 * 1024, 1, m8::protocol::service);
    return instance;
}

#ifdef M8EC_LIVENESS_SVC
struct LivenessSvc : fonas::Thread {

    static LivenessSvc &get_instance() {
        static LivenessSvc instance;
        return instance;
    }

private:
    LivenessSvc() : fonas::Thread("liveness", 1024, 1) {}

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
#endif // M8EC_LIVENESS_SVC

static bool init_hw_periphs() {
#if defined(STM32H750xx)
    ASSERT(periph::Uart4::get_instance().init());
    logger.info("UART4 OK");
#elif defined(STM32F411xE)
    ASSERT(periph::Uart1::get_instance().init());
    logger.info("UART1 OK");
#endif
    ASSERT(periph::UsbCdc::get_instance().init());
    logger.info("USB CDC OK");
    return true;
}

static bool init_sw_periphs() {
    ASSERT(Display::get_instance().init());
    logger.info("Display OK");
    return true;
}

static bool init_services() {
#ifdef M8EC_LIVENESS_SVC
    ASSERT(LivenessSvc::get_instance().Start());
#endif // M8EC_LIVENESS_SVC
    ASSERT(m8::protocol::Keys::Svc::get_instance().Start());
    logger.info("Keys::Svc::Service OK");
    ASSERT(m8::protocol::service.init());
    logger.info("m8::protocol::Service OK");
    return true;
}

void launch() {
    ASSERT(init_hw_periphs());
    ASSERT(init_sw_periphs());
    ASSERT(init_services());
}

} // namespace m8ec

extern "C" void platform_app() { m8ec::launch(); }

extern "C" void platform_print_freertos_stats(const char *buff) {
    const ili9341_text_attr_t attr{.font = &ili9341_font_trash80_stealth57,
                                   .fg_color = ILI9341_WHITE,
                                   .bg_color = ILI9341_BLACK,
                                   .origin = {.x = 0, .y = 0},
                                   .h_wrap = ILI9341_H_WRAP_OFF};
    ili9341_draw_string(m8ec::Display::get_instance().lcd(), attr, buff);
}
