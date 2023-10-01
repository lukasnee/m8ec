
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

#include "ILI9341/ili9341_font.h"

namespace m8ec::font {

/**
 * @brief stealth57 5x7 font by trash80 with some modifications.
 *
 * @details The TTF font file was converted into C array using LVGL Online Font Converter V 5.3 and hand-modified to fit ILI9341
 * font format. Unfortunatelly the LVGL converter does not center-align the monochrome glyphs, thus some fine tuning was
 * required. Additionally, '-' was widened to look more alike the font rendered on the original M8 device.
 *
 * @todo Add support for bold font.
 * @todo Center-align the monochrome glyphs.
 *
 * @ref https://lvgl.io/tools/font_conv_v5_3
 * @ref https://fontstruct.com/fontstructions/show/413734/stealth57
 */
extern ili9341_font_t const trash80_stealth57;

} // namespace m8ec::font