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

#include "lfsapp/lfsapp.h"

#include "SEGGER_SYSVIEW.h"
#include <reent.h> // required for _write_r
struct _reent;

#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/unistd.h>

#include <cerrno>
#include <cstdio>
#include <cstring>

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

constexpr size_t max_files = 4;

template <size_t N> struct FileTable {
    int alloc() {
        for (size_t i = 0; i < N; i++) {
            if (!this->files[i].active) {
                this->files[i].active = true;
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    void free(int fd) { this->files[fd].active = false; }

    lfs_file_t *get_lfs_file_of(int fd) {
        if (fd < 0 || fd >= static_cast<int>(N)) {
            return nullptr;
        }
        if (!this->files[fd].active) {
            return nullptr;
        }
        return &this->files[fd].file;
    }

private:
    struct {
        lfs_file_t file;
        bool active;
    } files[N];
};

static FileTable<max_files> file_table;

extern "C" int _open(char *path, int flags, int mode) {
    UNUSED(mode);
    extern lfs_t lfs;
    int lfs_flags = 0;
    if (flags & ~(O_RDONLY | O_WRONLY | O_RDWR | O_APPEND | O_CREAT | O_TRUNC | O_EXCL)) {
        errno = EINVAL;
        return -1;
    }
    const struct {
        int newlib_flag;
        int lfs_flag;
    } flag_map[] = {{O_RDONLY, LFS_O_RDONLY}, {O_WRONLY, LFS_O_WRONLY}, {O_RDWR, LFS_O_RDWR}, {O_APPEND, LFS_O_APPEND},
                    {O_CREAT, LFS_O_CREAT},   {O_TRUNC, LFS_O_TRUNC},   {O_EXCL, LFS_O_EXCL}};
    for (const auto &fm : flag_map) {
        if (flags & fm.newlib_flag) {
            lfs_flags |= fm.lfs_flag;
        }
    }
    static_assert(O_RDONLY == 0);
    if (lfs_flags == 0) {
        lfs_flags = LFS_O_RDONLY;
    }
    auto fd = file_table.alloc();
    if (fd < 0) {
        errno = ENFILE;
        return -1;
    }
    auto file = file_table.get_lfs_file_of(fd);
    if (!file) {
        file_table.free(fd);
        errno = EBADF;
        return -1;
    }
    int rc = lfs_file_open(&lfs, file, path, lfs_flags);
    if (rc < 0) {
        file_table.free(fd);
        errno = -rc;
        return -1;
    }
    return fd;
}

extern "C" int _close(int fd) {
    lfs_file_t *file = file_table.get_lfs_file_of(fd);
    if (!file) {
        errno = EBADF;
        return -1;
    }
    int rc = lfs_file_close(&lfs, file);
    if (rc < 0) {
        errno = -rc;
        return -1;
    }
    file_table.free(fd);
    return 0;
}

extern "C" int _read(int fd, char *ptr, int len) {
    lfs_file_t *file = file_table.get_lfs_file_of(fd);
    if (!file) {
        errno = EBADF;
        return -1;
    }
    int rc = lfs_file_read(&lfs, file, ptr, len);
    if (rc < 0) {
        errno = -rc;
        return -1;
    }
    return rc;
}

/**
 * @brief Standard output redirection to the screen
 */
extern "C" int _write(int fd, char *ptr, int len) {
    if (fd == STDIN_FILENO) {
        errno = EBADF;
        return -1;
    }
    if ((fd == STDOUT_FILENO) || (fd == STDERR_FILENO)) {
#ifdef SEGGER_SYSVIEW_ENABLED
        SEGGER_SYSVIEW_PrintData(ptr, len);
#else // #ifdef SEGGER_SYSVIEW_ENABLED
#if defined(STM32H750xx)
        using SerialDebug = m8ec::periph::Uart4;
#elif defined(STM32F411xE)
        using SerialDebug = m8ec::periph::Uart1;
#endif
        if (!SerialDebug::get_instance().write(reinterpret_cast<std::uint8_t *>(ptr), len)) {
            FONAS_PANIC();
        }
#endif // #ifdef SEGGER_SYSVIEW_ENABLED
        return len;
    }
    lfs_file_t *file = file_table.get_lfs_file_of(fd);
    if (!file) {
        errno = EBADF;
        return -1;
    }
    int rc = lfs_file_write(&lfs, file, ptr, len);
    if (rc < 0) {
        errno = -rc;
        return -1;
    }
    return rc;
}

extern "C" int _lseek(int fd, int ptr, int dir) {
    lfs_file_t *file = file_table.get_lfs_file_of(fd);
    if (!file) {
        errno = EBADF;
        return -1;
    }
    int rc = lfs_file_seek(&lfs, file, ptr, dir);
    if (rc < 0) {
        errno = -rc;
        return -1;
    }
    return rc;
}

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
