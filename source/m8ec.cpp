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

#include "usb_host.h"

#include "fonas/fonas.hpp"
#include "fonas/logger/logger.hpp"

#include <cstdio>

LOG_MODULE(m8ec, LOGGER_LEVEL_INFO);

namespace m8ec {
periph::UsbCdc &get_usb_cdc() {
    class UsbCdc : public periph::UsbCdc {
    public:
        static UsbCdc &get_instance() {
            static UsbCdc instance;
            return instance;
        }

        UsbCdc() : periph::UsbCdc(m8ec::Config::usb_cdc_rx_stream_buffer_size) {}
        bool is_ready() final {
            return usbh_ready();
        }
    };
    return UsbCdc::get_instance();
}
M8Display &get_display() {
    static M8Display instance(Display::get_instance());
    return instance;
}
m8::protocol::Service &get_m8_svc() {
    static m8::protocol::Service instance(get_display());
    return instance;
}
m8::protocol::Keys::Svc &get_key_svc() {
    static m8::protocol::Keys::Svc instance("keysSvc", 1, get_m8_svc());
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
    FONAS_ASSERT(periph::Uart4::get_instance().init());
    LOG_INFO("UART4 OK");
#elif defined(STM32F411xE)
    FONAS_ASSERT(periph::Uart1::get_instance().init());
    LOG_INFO("UART1 OK");
#endif
    FONAS_ASSERT(m8ec::get_usb_cdc().init());
    LOG_INFO("USB CDC OK");
    return true;
}

static bool init_sw_periphs() {
    FONAS_ASSERT(Display::get_instance().init());
    LOG_INFO("Display OK");
    return true;
}

static bool init_services() {
#ifdef M8EC_LIVENESS_SVC
    FONAS_ASSERT(LivenessSvc::get_instance().Start());
#endif // M8EC_LIVENESS_SVC
    FONAS_ASSERT(get_key_svc().Start());
    LOG_INFO("Keys::Svc::Service OK");
    FONAS_ASSERT(get_m8_svc().init());
    LOG_INFO("m8::protocol::Service OK");
    return true;
}

void launch() {
    FONAS_ASSERT(init_hw_periphs());
    FONAS_ASSERT(init_sw_periphs());
    FONAS_ASSERT(init_services());
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
