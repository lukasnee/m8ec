#include "m8ec/periph/UsbCdc.hpp"

#include "m8ec/m8/protocol.hpp"
#include "m8ec/m8ec.hpp"

#include "usbh_core.h"
#include "usbh_cdc_acm.h"
#include "stm32xxxx_hal.h"

#include <algorithm>

namespace m8ec::periph {

UsbCdc::UsbCdc(UBaseType_t rx_stream_buffer_size)
    : cpp_freertos::Thread("UsbCdcRx", 2048, configMAX_PRIORITIES + 1),
      rx_stream_buffer(rx_stream_buffer_size, sizeof(char)), usbh_cdc_acm{usbh_cdc_acm} {}

bool UsbCdc::init(struct usbh_cdc_acm *usbh_cdc_acm) {
    if (this->initialized) {
        LOG_ERROR("UsbCdc already initialized");
        return false;
    }
    if (!usbh_cdc_acm) {
        LOG_ERROR("UsbCdc usbh_cdc_acm is null");
        return false;
    }
    this->usbh_cdc_acm = usbh_cdc_acm;
    if (!this->fonas::EventDriven::Stream<fonas::EventDriven::StreamType::w>::init()) {
        LOG_ERROR("UsbCdc Stream init failed");
        return false;
    }
    this->initialized = true;
    return this->Start();
}

bool UsbCdc::is_ready() { return this->initialized && this->usbh_cdc_acm; }

void UsbCdc::Run() {
    while (true) {
        usbh_bulk_urb_fill(&this->usbh_cdc_acm->bulkin_urb, this->usbh_cdc_acm->hport, this->usbh_cdc_acm->bulkin,
                           this->rx_buff, this->usbh_cdc_acm->bulkin->wMaxPacketSize, 0xffffffff,
                           usbh_complete_callback, this);
        const auto ret = usbh_submit_urb(&this->usbh_cdc_acm->bulkin_urb);
        if (ret < 0) {
            LOG_ERROR("bulkin: %d", ret);
        }
        this->rx_sem.Take(portMAX_DELAY);
    }
}

void UsbCdc::usbh_complete_callback(void *arg, int nbytes) {
    UsbCdc *self = reinterpret_cast<UsbCdc *>(arg);
    LOG_SCOPE(self->logger_module);
    if (nbytes < 0) {
        LOG_ERROR("bulkin: %d", nbytes);
    }
    else if (static_cast<size_t>(nbytes) != self->rx_stream_buffer.send_from_isr(self->rx_buff, nbytes)) {
        LOG_ERROR("rx_stream_buffer.send_from_isr failed");
    }
    self->rx_sem.GiveFromISR(nullptr);
}

std::uint8_t UsbCdc::read() {
    std::uint8_t byte = 0;
    this->rx_stream_buffer.receive(&byte, sizeof(byte), portMAX_DELAY);
    return byte;
}

std::size_t UsbCdc::read(std::uint8_t *buffer, std::size_t bufferSize) {
    return this->rx_stream_buffer.receive(buffer, bufferSize, portMAX_DELAY);
}

bool UsbCdc::ll_init() { return true; /* hUsbHostFS is expected to be initialized in main.c */ }

bool UsbCdc::ll_write_async(const std::uint8_t *data, std::size_t size) {
    if (!this->initialized) {
        return false;
    }
    if (!this->is_ready()) {
        return false;
    }
    usbh_bulk_urb_fill(
        &this->usbh_cdc_acm->bulkout_urb, this->usbh_cdc_acm->hport, this->usbh_cdc_acm->bulkout,
        const_cast<std::uint8_t *>(data), size, 3000,
        [](void *arg, int nbytes) -> void {
            (void)arg;
            (void)nbytes;
            m8ec::get_usb_cdc().ll_async_write_completed_cb();
        },
        this->usbh_cdc_acm);
    const auto ret = usbh_submit_urb(&this->usbh_cdc_acm->bulkout_urb);
    if (ret < 0) {
        LOG_ERROR("bulkout: %d", ret);
        return false;
    }
    LOG_DEBUG("bulkout: %d", (unsigned int)this->usbh_cdc_acm->bulkout_urb.actual_length);
    return true;
}

bool UsbCdc::ll_deinit() {
    this->initialized = false;
    return true; /* handled in main.c */
}

} // namespace m8ec::periph
