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

#include "fonas/fonas.hpp"

namespace m8ec {

class KeysThread : public fonas::Thread {
public:
    using Thread::Thread;
    static KeysThread &get_instance();
    bool init();

private:
    void Run() final override;
};

} // namespace m8ec
