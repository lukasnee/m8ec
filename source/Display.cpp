/*
 * m8ec - Embedded Client for the Dirtywave M8 Headless device.
 * Copyright (C) 2023 Lukas Neverauskis https://github.com/lukasnee
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "m8ec/Display.hpp"

#include "lvgl.h"
#include "src/drivers/display/ili9341/lv_ili9341.h"

#include "m8ec/m8ec.h" // LOG
#include "m8ec/periph/Spi1.hpp"
#include "main.h"

#include "tickhook.hpp"

#ifdef M8EC_LVGL_DISPLAY_STARTUP_TEST
#include "lfsapp/lfsapp.h"

int lfs_ls(lfs_t *lfs, const char *path, char *buff, size_t buff_size) {
    auto printf = [&buff, &buff_size](const char *fmt, ...) -> void {
        if (buff_size == 0) {
            return;
        }
        va_list args;
        va_start(args, fmt);
        int len = vsnprintf(buff, buff_size, fmt, args);
        va_end(args);
        if (len < 0) {
            return;
        }
        if (static_cast<size_t>(len) >= buff_size) {
            len = buff_size - 1;
        }
        buff += len;
        buff_size -= len;
    };

    lfs_dir_t dir;
    int err = lfs_dir_open(lfs, &dir, path);
    if (err) {
        return err;
    }

    struct lfs_info info;
    while (true) {
        int res = lfs_dir_read(lfs, &dir, &info);
        if (res < 0) {
            return res;
        }

        if (res == 0) {
            break;
        }

        switch (info.type) {
        case LFS_TYPE_REG:
            printf("reg ");
            break;
        case LFS_TYPE_DIR:
            printf("dir ");
            break;
        default:
            printf("?   ");
            break;
        }

        static const char *prefixes[] = {"", "K", "M", "G"};
        for (int i = sizeof(prefixes) / sizeof(prefixes[0]) - 1; i >= 0; i--) {
            if (static_cast<int>(info.size) >= (1 << 10 * i) - 1) {
                printf("%*u%sB ", 4 - (i != 0), info.size >> 10 * i, prefixes[i]);
                break;
            }
        }

        printf("%s\n", info.name);
    }

    err = lfs_dir_close(lfs, &dir);
    if (err) {
        return err;
    }

    return 0;
}
#endif

namespace m8ec {

Display &Display::get_instance() {
    static Display instance;
    return instance;
}

class LvlgTickHook : public cpp_freertos::TickHook {
public:
    static LvlgTickHook &get_instance() {
        static LvlgTickHook instance;
        return instance;
    }

private:
    void Run() final { lv_tick_inc(1); }
};

class LvlgTimerHandler : public cpp_freertos::Thread {
public:
    static LvlgTimerHandler &get_instance() {
        static LvlgTimerHandler instance;
        return instance;
    }

private:
    LvlgTimerHandler() : cpp_freertos::Thread("lvlgTimHdlr", 1024, 1) {}
    void Run() final {
        while (true) {
            uint32_t time_till_next_ms = lv_timer_handler();
            if (time_till_next_ms == LV_NO_TIMER_READY) {
                time_till_next_ms = LV_DEF_REFR_PERIOD;
            }
            fonas::delay_ms(time_till_next_ms);
        }
    }
};

bool Display::init() {
    lv_init();

    LvlgTickHook::get_instance().Register();
    lv_tick_set_cb(xTaskGetTickCount);

    HAL_GPIO_WritePin(TFT_RESET_GPIO_Port, TFT_RESET_Pin, GPIO_PIN_RESET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(TFT_RESET_GPIO_Port, TFT_RESET_Pin, GPIO_PIN_SET);
    HAL_Delay(100);

    const uint32_t ili9341_hor_res = 240;
    const uint32_t ili9341_ver_res = 320;
    //  only one device on SPI bus - no need to toggle CS
    HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_RESET); // SPI CS active
    this->lv_display = lv_ili9341_create(
        ili9341_hor_res, ili9341_ver_res, LV_LCD_FLAG_BGR,
        [](lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, const uint8_t *param, size_t param_size) -> void {
            LV_UNUSED(disp);
            // HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_RESET); // SPI CS active
            HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_Pin, GPIO_PIN_RESET); // command mode
            if (!m8ec::periph::Spi1::get_instance().write((uint8_t *)cmd, cmd_size)) {
                // HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_SET); // SPI CS inactive
                return;
            }
            HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_Pin, GPIO_PIN_SET); //  data mode
            m8ec::periph::Spi1::get_instance().write_async((uint8_t *)param, param_size);
        },
        [](lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, uint8_t *param, size_t param_size) -> void {
            LV_UNUSED(disp);
            // HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_RESET); // SPI CS active
            HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_Pin, GPIO_PIN_RESET); // command mode
            if (!m8ec::periph::Spi1::get_instance().write((uint8_t *)cmd, cmd_size)) {
                // HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_SET); // SPI CS inactive
                return;
            }
            HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_Pin, GPIO_PIN_SET); //  data mode
            m8ec::periph::Spi1::get_instance().write_async((uint8_t *)param, param_size);
        });
    lv_display_set_flush_wait_cb(this->lv_display, [](lv_display_t *disp) -> void {
        m8ec::periph::Spi1::get_instance().write_await();
        // HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_SET); // SPI CS inactive
    });

    lv_display_set_rotation(this->lv_display, LV_DISPLAY_ROTATION_90);

    const uint32_t buf_size = ili9341_hor_res * ili9341_ver_res *
                              lv_color_format_get_size(lv_display_get_color_format(this->lv_display)) / 10;

    void *buf1 = lv_malloc(buf_size);
    if (buf1 == NULL) {
        LOG("display buf1 malloc failed");
        return false;
    }

    void *buf2 = lv_malloc(buf_size);
    if (buf2 == NULL) {
        LOG("display buf2 malloc failed");
        lv_free(buf1);
        return false;
    }
    lv_display_set_buffers(this->lv_display, buf1, buf2, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);

    lv_obj_t *scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_100, 0);

#ifdef M8EC_LVGL_DISPLAY_STARTUP_TEST

    // lv_obj_t *label_obj = lv_label_create(scr);
    // lv_obj_set_align(label_obj, LV_ALIGN_CENTER);
    // lv_obj_set_height(label_obj, LV_SIZE_CONTENT);
    // lv_obj_set_width(label_obj, LV_SIZE_CONTENT);
    // lv_obj_set_style_text_font(label_obj, &lv_font_montserrat_14, 0);
    // lv_obj_set_style_text_color(label_obj, lv_color_white(), 0);
    // lv_label_set_text(label_obj, "Hello World!");

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x003a57), LV_PART_MAIN);

    /*Create a white label, set its text and align it to the center*/
    lv_obj_t *label = lv_label_create(lv_screen_active());
    char buf[2 * 1024];
#if 1
    lfs_ls(&lfs, "/", buf, sizeof(buf));
#else
    auto file = fopen("main.lua", "r");
    if (file) {
        fread(buf, 1, sizeof(buf), file);
        fclose(file);
    }
#endif
    buf[sizeof(buf) - 1] = '\0';
    lv_label_set_text(label, buf);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(label, &trash80_stealth57, 0);
    lv_obj_set_style_text_line_space(label, 1, 0);
    lv_obj_set_scrollbar_mode(label, LV_SCROLLBAR_MODE_OFF);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, label);
    lv_anim_set_exec_cb(
        &a, (lv_anim_exec_xcb_t)[](void *obj, int32_t color) {
            // lv_obj_set_style_bg_color(
            //     lv_screen_active(), lv_color_make(0x80 - color / 4, 0x80 + color / 2, 0x80 + color / 3),
            //     LV_PART_MAIN);
            // lv_obj_set_style_text_color((lv_obj_t *)obj, lv_color_hex(color), LV_PART_MAIN);
            lv_obj_align((lv_obj_t *)obj, LV_ALIGN_TOP_LEFT, color / 0xFFFFF, color);
        });
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_time(&a, 4000);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);
#endif
    return LvlgTimerHandler::get_instance().Start();
}

lv_display_t *Display::lcd() { return this->lv_display; }

} // namespace m8ec
