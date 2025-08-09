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

#include "ln/ln.hpp"
#include "ln/logger/logger.hpp"

#include <cstdio>

LOG_MODULE(m8ec, LOGGER_LEVEL_INFO);

namespace m8ec {
periph::UsbCdc &get_usb_cdc() {
    class UsbCdc : public periph::UsbCdc {
    public:
        using periph::UsbCdc::UsbCdc;
        static UsbCdc &get_instance() {
            static FreeRTOS::StaticStreamBuffer<m8ec::Config::usb_cdc_rx_stream_buffer_size> rx_stream_buffer;
            static UsbCdc instance(rx_stream_buffer);
            return instance;
        }
        bool is_ready() final { return usbh_ready(); }
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
// #define M8EC_LIVENESS_DISPLAY_INDICATOR

struct IdleMonitoringSvc : FreeRTOS::Task {

    static IdleMonitoringSvc &get_instance() {
        static IdleMonitoringSvc instance;
        return instance;
    }

    bool init() { return true; }

private:
    IdleMonitoringSvc() : FreeRTOS::Task(tskIDLE_PRIORITY, 1024, "idle_mon") {}

    virtual void taskFunction() final {
        while (true) {
            ln_logger_flush_buffer();
#ifdef M8EC_LIVENESS_DISPLAY_INDICATOR
            static const char loadingChars[] = {'|', '/', '-', '\\'};
            static uint8_t loadingCharIndex = 0;
            ili9341_draw_char(Display::get_instance().lcd(), attr, loadingChars[loadingCharIndex]);
            loadingCharIndex = (loadingCharIndex + 1) % sizeof(loadingChars);
#endif
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
};

static bool init_hw_periphs() {
#if defined(STM32H750xx)
    LN_ASSERT(periph::Uart4::get_instance().init());
    LOG_INFO("UART4 OK");
#elif defined(STM32F411xE)
    LN_ASSERT(periph::Uart1::get_instance().init());
    LOG_INFO("UART1 OK");
#endif
    LN_ASSERT(m8ec::get_usb_cdc().init());
    LOG_INFO("USB CDC OK");
    return true;
}

static bool init_sw_periphs() {
    LN_ASSERT(Display::get_instance().init());
    LOG_INFO("Display OK");
    return true;
}

static bool init_services() {
    LN_ASSERT(IdleMonitoringSvc::get_instance().init());
    LN_ASSERT(get_key_svc().init());
    LOG_INFO("Keys::Svc::Service OK");
    LN_ASSERT(get_m8_svc().init());
    LOG_INFO("m8::protocol::Service OK");
    return true;
}

void launch() {
    LN_ASSERT(init_hw_periphs());
    LN_ASSERT(init_sw_periphs());
    LN_ASSERT(init_services());
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
