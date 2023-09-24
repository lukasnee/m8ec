#include "m8ec/periph/UsbCdc.hpp"

#include "m8ec/m8ec.hpp"

#include "stm32f4xx_hal.h"
#include "usb_host.h"
#include "usbh_cdc.h"

#include <algorithm>

extern USBH_HandleTypeDef hUsbHostFS; // TODO decouple

namespace m8ec::periph {

UsbCdc &UsbCdc::get_instance() {
    static UsbCdc instance(m8ec::Config::usb_cdc_rx_stream_buffer_size);
    return instance;
}

UsbCdc::UsbCdc(UBaseType_t rx_stream_buffer_size) : rx_stream_buffer(rx_stream_buffer_size, sizeof(char)) {}

bool UsbCdc::ready() const { return this->initialized && m8ec_virtual_com_ready(); }

std::uint8_t UsbCdc::read() {
    std::uint8_t byte = 0;
    this->rx_stream_buffer.receive(&byte, sizeof(byte), portMAX_DELAY);
    return byte;
}

bool UsbCdc::ll_init() {
    this->initialized = true;
    return true; /* hUsbHostFS is expected to be initialized in main.c */
}

bool UsbCdc::ll_async_write(const std::uint8_t *data, std::size_t size) {
    if (!data) {
        return false;
    }
    if (size == 0) {
        return false;
    }
    if (!this->ready()) {
        return false;
    }
    const auto status = USBH_CDC_Transmit(&hUsbHostFS, const_cast<std::uint8_t *>(data), size);
    if (USBH_OK != status) {
        printf("error: USBH_CDC_Transmit: %d\n", status);
        return false;
    }
    return true;
}

bool UsbCdc::ll_deinit() {
    this->initialized = false;
    return true; /* handled in main.c */
}

bool UsbCdc::ll_rx_input(const uint8_t *data, size_t size) {
    if (!data) {
        return false;
    }
    if (size == 0) {
        return false;
    }
    if (!this->ready()) {
        return false;
    }
    if (size != this->rx_stream_buffer.send(data, size, portMAX_DELAY)) {
        printf("rx_stream_buffer.send failed\n");
        return false;
    }
    return true;
}

} // namespace m8ec::periph

extern "C" void USBH_CDC_TransmitCallback(USBH_HandleTypeDef *phost) {
    if (phost == &hUsbHostFS) {
        m8ec::periph::UsbCdc::get_instance().ll_async_write_completed_cb();
    }
}

extern "C" void USBH_CDC_ReceiveCallback(USBH_HandleTypeDef *phost, const uint8_t *data, uint32_t size) {
    if (phost == &hUsbHostFS) {
        m8ec::periph::UsbCdc::get_instance().ll_rx_input(data, size);
    }
}
