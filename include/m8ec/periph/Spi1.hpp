/*
 * m8ec - Embedded Client for the Dirtywave M8 Headless device.
 * Copyright (C) 2023 Lukas Neverauskis https://github.com/lukasnee
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#pragma once

#include "fonas/EventDrivenSpi.hpp"

namespace m8ec::periph {

class Spi1 : public fonas::EventDrivenSpi {

public:
    static Spi1 &get_instance();

protected:
    bool ll_init() final;
    bool ll_read_async(std::uint8_t *data, std::size_t size) final;
    bool ll_write_async(const std::uint8_t *data, std::size_t size) final;
    bool ll_read_write_async(std::uint8_t *rd_data, const std::uint8_t *wr_data, std::size_t size) final;
    bool ll_busy_writing() final;
    bool ll_busy_reading() final;
    bool ll_deinit() final;
};

} // namespace m8ec::periph
