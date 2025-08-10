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

#include "m8ec/m8/protocol.hpp"

#include "FreeRTOS/Task.hpp"

namespace m8ec::services {

static constexpr size_t idle_monitoring_svc_stack_size = 1024;

struct IdleMonitoringSvc : FreeRTOS::StaticTask<idle_monitoring_svc_stack_size> {

    static IdleMonitoringSvc &get_instance() {
        static IdleMonitoringSvc instance;
        return instance;
    }

    bool init() { return true; }

private:
    IdleMonitoringSvc();

    virtual void taskFunction();
};

m8::protocol::Service &get_m8_svc();
m8::protocol::Keys::Svc &get_key_svc();

bool init();
} // namespace m8ec::services
