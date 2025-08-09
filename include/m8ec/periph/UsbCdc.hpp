#pragma once

#include "ln/drivers/EventDrivenStream.hpp"
#include "ln/logger/logger.hpp"

#include "FreeRTOS/StreamBuffer.hpp"

namespace m8ec::periph {

class UsbCdc : public ln::drivers::EventDriven::Stream<ln::drivers::EventDriven::StreamType::w> {

public:
    UsbCdc(FreeRTOS::StreamBufferBase &rx_stream_buffer);

    /**
     * @brief Check if low-level implementation is ready for streaming.
     */
    virtual bool is_ready() = 0;

    std::uint8_t read();
    std::size_t read(std::uint8_t *buffer, std::size_t bufferSize);

    bool ll_rx_input(const uint8_t *data, size_t size);

protected:
    bool ll_init() final override;
    bool ll_write_async(const std::uint8_t *data, std::size_t size) final override;
    bool ll_deinit() final override;

private:
    using ln::drivers::EventDriven::Stream<ln::drivers::EventDriven::StreamType::w>::ll_write_async;

    FreeRTOS::StreamBufferBase &rx_stream_buffer;
    bool initialized = false;

    LOG_MODULE_CLASS_MEMBER(m8ec::periph::UsbCdc, LOGGER_LEVEL_INFO);
};

} // namespace m8ec::periph
