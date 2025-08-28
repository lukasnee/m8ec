/*
 * m8ec - Embedded Client for the Dirtywave M8 Headless device.
 * Copyright (C) 2023 Lukas Neverauskis https://github.com/lukasnee
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

// The newlib syscalls implementation

#if defined(STM32H750xx)
#include "m8ec/periph/Uart4.hpp"
#elif defined(STM32F411xE)
#include "m8ec/periph/Uart1.hpp"
#endif

#include "ln/syscalls/littlefs.hpp"

#include "SEGGER_SYSVIEW.h"

#include <sys/stat.h>   // for struct stat
#include <sys/unistd.h> // for STDIN_FILENO, etc.

#include <cerrno>

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

extern "C" int _open(char *path, int flags, int mode) {
    UNUSED(mode);
    return ln::syscalls::littlefs::open(path, flags);
}

extern "C" int _close(int fd) { return ln::syscalls::littlefs::close(fd); }

extern "C" int _read(int fd, char *ptr, int len) { return ln::syscalls::littlefs::read(fd, ptr, len); }

/**
 * @brief Standard output redirection to the screen
 */
extern "C" int _write(int fd, char *ptr, int len) {
    if ((fd == STDOUT_FILENO) || (fd == STDERR_FILENO)) {
#ifdef SEGGER_SYSVIEW_ENABLED
        SEGGER_SYSVIEW_PrintData(ptr, len);
#else // #ifdef SEGGER_SYSVIEW_ENABLED
#if defined(STM32H750xx)
        using SerialDebug = m8ec::periph::Uart4;
#elif defined(STM32F411xE)
        using SerialDebug = m8ec::periph::Uart1;
#endif
        SerialDebug::get_instance().write(reinterpret_cast<std::uint8_t *>(ptr), len);
#endif // #ifdef SEGGER_SYSVIEW_ENABLED
        return len;
    }
    return ln::syscalls::littlefs::write(fd, ptr, len);
}

extern "C" int _lseek(int fd, int ptr, int dir) { return ln::syscalls::littlefs::lseek(fd, ptr, dir); }

extern "C" int _stat(char *file, struct stat *st) {
    UNUSED(file);
    UNUSED(st);
    errno = ENOENT;
    return -1;
}

extern "C" int _fstat(int fd, struct stat *st) {
    UNUSED(fd);
    UNUSED(st);
    errno = EBADF;
    return -1;
}

extern "C" int _link(char *old, char *new_) {
    UNUSED(old);
    UNUSED(new_);
    errno = EMLINK;
    return -1;
}

extern "C" int _unlink(char *name) {
    UNUSED(name);
    errno = ENOENT;
    return -1;
}

extern "C" int _isatty(int fd) {
    UNUSED(fd);
    return 1;
}

extern "C" int _wait(int *status) {
    UNUSED(status);
    errno = ECHILD;
    return -1;
}

extern "C" clock_t _times(struct tms *tp) {
    UNUSED(tp);
    return -1;
}

extern "C" int _gettimeofday(struct timeval *tv, void *tz) {
    UNUSED(tv);
    UNUSED(tz);
    errno = EINVAL;
    return -1;
}

extern "C" int _fork(void) {
    errno = EAGAIN;
    return -1;
}

extern "C" int _execve(char *name, char **argv, char **env) {
    UNUSED(name);
    UNUSED(argv);
    UNUSED(env);
    errno = ENOMEM;
    return -1;
}

extern "C" int _getpid(void) { return 1; }

extern "C" int _kill(int pid, int sig) {
    UNUSED(pid);
    UNUSED(sig);
    errno = EINVAL;
    return -1;
}
