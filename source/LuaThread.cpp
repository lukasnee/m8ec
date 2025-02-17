/*
 * m8ec - Embedded Client for the Dirtywave M8 Headless device.
 * Copyright (C) 2023 Lukas Neverauskis https://github.com/lukasnee
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "LuaThread.hpp"
#include "m8ec/Display.hpp"
#include "m8ec/m8ec.h"
#include "lfsapp/lfsapp.h"

extern "C"
{
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}

LuaThread &LuaThread::get_instance() {
    static LuaThread instance;
    return instance;
}

LuaThread::LuaThread() : fonas::Thread("lua", 12 * 1024, 1) {}

void LuaThread::Run() {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    lua_pushcfunction(L, ([](lua_State *L) -> int {
                          fonas::delay_ms(luaL_checkinteger(L, 1));
                          return 0;
                      }));
    lua_setglobal(L, "delay_ms");
    lua_pushcfunction(L, ([](lua_State *L) -> int {
                          lua_pushinteger(L, fonas::get_uptime_ms());
                          return 1;
                      }));
    lua_setglobal(L, "get_uptime_ms");
    lua_pushcfunction(L, ([](lua_State *L) -> int {
                          ili9341_draw_pixel(
                              m8ec::Display::get_instance().lcd(),
                              __ILI9341_COLOR565(luaL_checkinteger(L, 1), luaL_checkinteger(L, 2), luaL_checkinteger(L, 3)),
                              (int16_t)luaL_checkinteger(L, 4), (int16_t)luaL_checkinteger(L, 5));
                          return 0;
                      }));
    lua_setglobal(L, "ili9341_draw_pixel");
    lua_pushcfunction(L, ([](lua_State *L) -> int {
                          const ili9341_text_attr_t attr = {
                              .font = &ili9341_font_trash80_stealth57,
                              .fg_color = ILI9341_WHITE,
                              .bg_color = ILI9341_BLACK,
                              .origin = {.x = (uint16_t)luaL_checkinteger(L, 1), .y = (uint16_t)luaL_checkinteger(L, 2)},
                              .h_wrap = ILI9341_H_WRAP_OFF};
                          ili9341_draw_string(m8ec::Display::get_instance().lcd(), attr, luaL_checkstring(L, 3));
                          return 0;
                      }));
    lua_setglobal(L, "ili9341_draw_string");
    const char *lua_script_path = "main.lua";
    while (true) {
        if (luaL_dofile(L, lua_script_path)) {
            ili9341_draw_string(
                m8ec::Display::get_instance().lcd(),
                {.font = &ili9341_font_trash80_stealth57,
                 .fg_color = ILI9341_RED,
                 .bg_color = ILI9341_BLACK,
                 .origin = {.x = 0,
                            .y = (uint16_t)(ili9341_font_trash80_stealth57.height + ili9341_font_trash80_stealth57.h_spacing)},
                 .h_wrap = 0},
                lua_tostring(L, -1));
            LOG("error: %s\n", lua_tostring(L, -1));
        }
        lfs_file_t file;
        lfs_file_open(&lfs, &file, lua_script_path, LFS_O_RDONLY);
        ili9341_pos_t pos = {.x = 0, .y = 0};
        for (char buf[64]; lfs_file_read(&lfs, &file, buf, sizeof(buf) - 1) > 0;) {
            buf[sizeof(buf) - 1] = '\0';
            pos = ili9341_draw_string(m8ec::Display::get_instance().lcd(),
                                      {.font = &ili9341_font_trash80_stealth57,
                                       .fg_color = ILI9341_WHITE,
                                       .bg_color = ILI9341_BLACK,
                                       .origin = pos,
                                       .h_wrap = 0},
                                      buf);
        }
        lfs_file_close(&lfs, &file);
        fonas::delay_ms(1000);
    }
    lua_close(L);
}