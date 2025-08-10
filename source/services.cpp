/*
 * m8ec - Embedded Client for the Dirtywave M8 Headless device.
 * Copyright (C) 2023 Lukas Neverauskis https://github.com/lukasnee
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "m8ec/services.hpp"
#include "m8ec/drivers.hpp"
#include "m8ec/config.hpp"

#include "ln/logger/logger.hpp"

LOG_MODULE(m8ec::services, LOGGER_LEVEL_INFO);

using namespace std::chrono_literals;

namespace m8ec::services {

IdleMonitoringSvc::IdleMonitoringSvc()
    : FreeRTOS::StaticTask<idle_monitoring_svc_stack_size>(tskIDLE_PRIORITY, "idle_mon") {}

void IdleMonitoringSvc::taskFunction() {
    while (true) {
        ln_logger_flush_buffer();
        if constexpr (config::liveness_display_indicator) {
            const ili9341_text_attr_t attr{.font = &ili9341_font_trash80_stealth57,
                                           .fg_color = ILI9341_WHITE,
                                           .bg_color = ILI9341_BLACK,
                                           .origin = {.x = 0, .y = 0},
                                           .h_wrap = ILI9341_H_WRAP_OFF};
            static const char loadingChars[] = {'|', '/', '-', '\\'};
            static uint8_t loadingCharIndex = 0;
            ili9341_draw_char(drivers::DisplayILI9341::get_instance().get_ili9341_t(), attr,
                              loadingChars[loadingCharIndex]);
            loadingCharIndex = (loadingCharIndex + 1) % sizeof(loadingChars);
        }
        this->delay(100ms);
    }
}

m8::protocol::Service &get_m8_svc() {
    static m8::protocol::Service instance(drivers::get_display());
    return instance;
}
m8::protocol::Keys::Svc &get_key_svc() {
    static m8::protocol::Keys::Svc instance("keysSvc", 1, get_m8_svc());
    return instance;
}

bool init() {
    LN_ASSERT(services::IdleMonitoringSvc::get_instance().init());
    LN_ASSERT(services::get_key_svc().init());
    LOG_INFO("Keys::Svc::Service OK");
    LN_ASSERT(services::get_m8_svc().init());
    LOG_INFO("m8::protocol::Service OK");
    return true;
}

} // namespace m8ec::services