#pragma once

#include "fonas/EventDrivenStream.hpp"

namespace m8ec::periph {

class UsbCdc : public fonas::EventDriven::Stream<fonas::EventDriven::StreamType::w> {

public:
    static UsbCdc &get_instance();

    UsbCdc(UBaseType_t rx_stream_buffer_size);

    bool ready() const;

    std::uint8_t read();
    std::size_t read(std::uint8_t *buffer, std::size_t bufferSize);

    bool ll_rx_input(const uint8_t *data, size_t size);

protected:
    bool ll_init() final override;
    bool ll_write_async(const std::uint8_t *data, std::size_t size) final override;
    bool ll_deinit() final override;

private:
    using fonas::EventDriven::Stream<fonas::EventDriven::StreamType::w>::ll_write_async;

    fonas::StreamBuffer rx_stream_buffer;
    bool initialized = false;
};

} // namespace m8ec::periph
