#include "m8ec/periph/UsbCdc.hpp"

#include "m8ec/m8/protocol.hpp"
#include "m8ec/m8ec.hpp"

#include "stm32xxxx_hal.h"
#include "usb_host.h"
#include "usbh_cdc.h"

#include <algorithm>

extern USBH_HandleTypeDef hUsbHostFS; // TODO decouple

namespace m8ec::periph {

UsbCdc::UsbCdc(UBaseType_t rx_stream_buffer_size) : rx_stream_buffer(rx_stream_buffer_size, sizeof(char)) {}

std::uint8_t UsbCdc::read() {
    std::uint8_t byte = 0;
    this->rx_stream_buffer.receive(&byte, sizeof(byte), portMAX_DELAY);
    return byte;
}

std::size_t UsbCdc::read(std::uint8_t *buffer, std::size_t bufferSize) {
    return this->rx_stream_buffer.receive(buffer, bufferSize, portMAX_DELAY);
}

bool UsbCdc::ll_init() {
    this->initialized = true;
    return true; /* hUsbHostFS is expected to be initialized in main.c */
}

bool UsbCdc::ll_write_async(const std::uint8_t *data, std::size_t size) {
    if (!this->initialized) {
        return false;
    }
    if (!this->is_ready()) {
        return false;
    }
    const auto status = USBH_CDC_Transmit(&hUsbHostFS, const_cast<std::uint8_t *>(data), size);
    if (USBH_OK != status) {
        LOG_ERROR("USBH_CDC_Transmit: %d", status);
        return false;
    }
    return true;
}

bool UsbCdc::ll_deinit() {
    this->initialized = false;
    return true; /* handled in main.c */
}

bool UsbCdc::ll_rx_input(const uint8_t *data, size_t size) {
    if (!this->initialized) {
        return false;
    }
    if (!this->is_ready()) {
        return false;
    }
    if (size != this->rx_stream_buffer.send(data, size, portMAX_DELAY)) {
        LOG_ERROR("rx_stream_buffer.send failed");
        return false;
    }
    return true;
}

} // namespace m8ec::periph

extern "C" void USBH_CDC_TransmitCallback(USBH_HandleTypeDef *phost) {
    if (phost == &hUsbHostFS) {
        m8ec::get_usb_cdc().ll_async_write_completed_cb();
    }
}

extern "C" void USBH_CDC_ReceiveCallback(USBH_HandleTypeDef *phost, const uint8_t *data, uint32_t size) {
    if (phost == &hUsbHostFS) {
        m8ec::get_usb_cdc().ll_rx_input(data, size);
    }
}
