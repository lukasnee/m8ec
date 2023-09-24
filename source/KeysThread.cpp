/*
 * m8ec - Embedded Client for the Dirtywave M8 Headless device.
 * Copyright (C) 2023 Lukas Neverauskis https://github.com/lukasnee
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "m8ec/KeysThread.hpp"

#include "m8ec/Keys.hpp"
#include "m8ec/m8_protocol.hpp"
#include "m8ec/m8ec.hpp"

#include <cstdio>

namespace m8ec {

KeysThread &KeysThread::get_instance() {
    static KeysThread instance("keys", 2 * 1024, 1);
    return instance;
}

bool KeysThread::init() { return this->Start(); }

void print_keys_change(const IKeys::State &prev_keys_state, const IKeys::State &keys_state) {

    for (const auto &key : IKeys::keys) {
        if (prev_keys_state.get(key) != keys_state.get(key)) {
            printf("%s%c\n", IKeys::key_to_string(key), (keys_state.get(key) ? '+' : '-'));
        }
    }
}

void KeysThread::Run() {
    while (true) {
        Keys::getInstance().init();
        IKeys::State prev_keys_state = {};
        while (true) {
            const IKeys::State keys_state = Keys::getInstance().getState();
            if (prev_keys_state.underlying != keys_state.underlying) {
                m8_protocol::send_keys_state(keys_state);
                if (m8ec::Config::debug_keys) {
                    print_keys_change(prev_keys_state, keys_state);
                }
                prev_keys_state = keys_state;
            }
            Thread::DelayUntil(m8ec::Config::keys_refresh_period);
        }
    }
}

} // namespace m8ec
