#pragma once

#include "fonas/EventDrivenStream.hpp"
#include "fonas/StreamBuffer.hpp"
#include "fonas/logger/logger.hpp"

#include "usbh_core.h"
#include "usbh_cdc_acm.h"

namespace m8ec::periph {

class UsbCdc : public fonas::EventDriven::Stream<fonas::EventDriven::StreamType::w>, private cpp_freertos::Thread {

public:
    UsbCdc(UBaseType_t rx_stream_buffer_size);

    bool init(struct usbh_cdc_acm *usbh_cdc_acm);

    /**
     * @brief Check if low-level implementation is ready for streaming.
     */
    bool is_ready();

    std::uint8_t read();
    std::size_t read(std::uint8_t *buffer, std::size_t bufferSize);

protected:
    bool ll_init() final override;
    bool ll_write_async(const std::uint8_t *data, std::size_t size) final override;
    bool ll_deinit() final override;

private:
    void Run() final;

    static void usbh_complete_callback(void *arg, int nbytes);

    using fonas::EventDriven::Stream<fonas::EventDriven::StreamType::w>::ll_write_async;

    fonas::StreamBuffer rx_stream_buffer;
    bool initialized = false;

    LOG_MODULE_CLASS_MEMBER(m8ec::periph::UsbCdc, LOGGER_LEVEL_INFO);

    struct usbh_cdc_acm *usbh_cdc_acm = nullptr;
    uint8_t rx_buff[64];
    cpp_freertos::BinarySemaphore rx_sem{};
};

} // namespace m8ec::periph
