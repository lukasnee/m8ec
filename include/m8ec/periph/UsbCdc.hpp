#pragma once

#include "fonas/EventDrivenWriter.hpp"

namespace m8ec::periph {

class UsbCdc : public fonas::EventDrivenWriter {

public:
    static UsbCdc &get_instance();

    UsbCdc(UBaseType_t rx_stream_buffer_size);

    bool ready() const;

    std::uint8_t read();

    bool ll_rx_input(const uint8_t *data, size_t size);

protected:
    bool ll_init() final override;
    bool ll_async_write(const std::uint8_t *data, std::size_t size) final override;
    bool ll_deinit() final override;

private:
    using fonas::EventDrivenWriter::ll_async_write;
    using fonas::EventDrivenWriter::ll_async_write_completed_cb_from_isr;

    fonas::StreamBuffer rx_stream_buffer;
    bool initialized = false;
};

} // namespace m8ec::periph
