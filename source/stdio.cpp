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

#ifdef SEGGER_SYSVIEW
#include <reent.h>  // required for _write_r
#include "SEGGER_RTT.h"
struct _reent;
#endif // SEGGER_SYSVIEW

#include <cstdio>
#include <errno.h>
#include <sys/unistd.h>

/**
 * @brief Standard output redirection to the screen
 */
extern "C" int _write(int fd, char *ptr, int len) {
#ifdef SEGGER_SYSVIEW
      SEGGER_RTT_Write(0, ptr, len);
      return len;
#else
    // return 0;
    if ((fd != STDOUT_FILENO) && (fd != STDERR_FILENO)) {
        errno = EBADF;
        return -1;
    }
    if (!m8ec::periph::Uart1::get_instance().write(reinterpret_cast<std::uint8_t *>(ptr), len)) {
        FONAS_PANIC();
    }
    return len;
#endif
}

#ifdef SEGGER_SYSVIEW
extern "C" _ssize_t _write_r(struct _reent *r, int file, const void *ptr, size_t len) {
  (void) file;  /* Not used, avoid warning */
  (void) r;     /* Not used, avoid warning */
  SEGGER_RTT_Write(0, ptr, len);
  return len;
}
#endif // SEGGER_SYSVIEW
