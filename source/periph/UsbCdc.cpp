#include "m8ec/periph/UsbCdc.hpp"

#include "m8ec/m8ec.hpp"

#include "stm32f4xx_hal.h"
#include "usbh_cdc_acm.h"
#include "usbh_core.h"

#include <algorithm>

USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t cdc_buffer[512];

struct usbh_urb cdc_bulkin_urb;
struct usbh_urb cdc_bulkout_urb;

void usbh_cdc_acm_callback(void *arg, int nbytes) {
    struct usbh_cdc_acm *cdc_acm_class = (struct usbh_cdc_acm *)arg;
    if (nbytes > 0) {
        for (size_t i = 0; i < nbytes; i++) {
            USB_LOG_RAW("0x%02x ", cdc_buffer[i]);
        }
        USB_LOG_RAW("nbytes:%d\r\n", nbytes);
    }
}

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

std::size_t UsbCdc::read(std::uint8_t *buffer, std::size_t bufferSize) {
    return this->rx_stream_buffer.receive(buffer, bufferSize, 0); // TODO maybe 1ms
}

bool UsbCdc::ll_init() {
    this->initialized = true;

    int ret;
    struct usbh_cdc_acm *cdc_acm_class;

    cdc_acm_class = (struct usbh_cdc_acm *)usbh_find_class_instance("/dev/ttyACM0");
    if (cdc_acm_class == NULL) {
        USB_LOG_RAW("do not find /dev/ttyACM0\r\n");
        usb_osal_msleep(1000);
        continue;
    }
    memset(cdc_buffer, 0, 512);

    usbh_bulk_urb_fill(&cdc_bulkin_urb, cdc_acm_class->bulkin, cdc_buffer, 64, 3000, NULL, NULL);
    ret = usbh_submit_urb(&cdc_bulkin_urb);
    if (ret < 0) {
        USB_LOG_RAW("bulk in error,ret:%d\r\n", ret);
    }
    else {
        USB_LOG_RAW("recv over:%d\r\n", cdc_bulkin_urb.actual_length);
        for (size_t i = 0; i < cdc_bulkin_urb.actual_length; i++) {
            USB_LOG_RAW("0x%02x ", cdc_buffer[i]);
        }
    }

    USB_LOG_RAW("\r\n");
    const uint8_t data1[10] = {0x02, 0x00, 0x00, 0x00, 0x02, 0x02, 0x08, 0x14};

    memcpy(cdc_buffer, data1, 8);
    usbh_bulk_urb_fill(&cdc_bulkout_urb, cdc_acm_class->bulkout, cdc_buffer, 8, 3000, NULL, NULL);
    ret = usbh_submit_urb(&cdc_bulkout_urb);
    if (ret < 0) {
        USB_LOG_RAW("bulk out error,ret:%d\r\n", ret);
    }
    else {
        USB_LOG_RAW("send over:%d\r\n", cdc_bulkout_urb.actual_length);
    }

    usbh_bulk_urb_fill(&cdc_bulkin_urb, cdc_acm_class->bulkin, cdc_buffer, 64, 3000, usbh_cdc_acm_callback, cdc_acm_class);
    ret = usbh_submit_urb(&cdc_bulkin_urb);
    if (ret < 0) {
        USB_LOG_RAW("bulk in error,ret:%d\r\n", ret);
    }
    else {
    }

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
