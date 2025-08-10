/*
 * m8ec - Embedded Client for the Dirtywave M8 Headless device.
 * Copyright (C) 2023 Lukas Neverauskis https://github.com/lukasnee
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "m8ec/drivers.hpp"

#include "ln/logger/logger.hpp"

#include "usb_host.h" // for usbh_ready()

LOG_MODULE(m8ec::drivers, LOGGER_LEVEL_INFO);

#if defined(STM32H750xx)
#include "m8ec/periph/Uart4.hpp"
#elif defined(STM32F411xE)
#include "m8ec/periph/Uart1.hpp"
#endif

namespace m8ec::drivers {
periph::UsbCdc &get_usb_cdc() {
    class UsbCdc : public periph::UsbCdc {
    public:
        using periph::UsbCdc::UsbCdc;
        static UsbCdc &get_instance() {
            static FreeRTOS::StaticStreamBuffer<config::usb_cdc_rx_stream_buffer_size> rx_stream_buffer;
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

bool init() {
#if defined(STM32H750xx)
    LN_ASSERT(periph::Uart4::get_instance().init());
    LOG_INFO("UART4 OK");
#elif defined(STM32F411xE)
    LN_ASSERT(periph::Uart1::get_instance().init());
    LOG_INFO("UART1 OK");
#endif
    LN_ASSERT(drivers::get_usb_cdc().init());
    LOG_INFO("USB CDC OK");
    return true;
}

} // namespace m8ec::drivers