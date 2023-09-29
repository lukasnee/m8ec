/*
 * m8ec - Embedded Client for the Dirtywave M8 Headless device.
 * Copyright (C) 2023 Lukas Neverauskis https://github.com/lukasnee
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "m8ec/periph/Uart1.hpp"

#include <cstdio>
#include <errno.h>
#include <sys/unistd.h>

/**
 * @brief Standard output redirection to the screen
 */
extern "C" int _write(int fd, char *ptr, int len) {
    // return 0;
    if ((fd != STDOUT_FILENO) && (fd != STDERR_FILENO)) {
        errno = EBADF;
        return -1;
    }
    if (!m8ec::periph::Uart1::get_instance().write(reinterpret_cast<std::uint8_t *>(ptr), len)) {
        FONAS_PANIC();
    }
    return len;
}
