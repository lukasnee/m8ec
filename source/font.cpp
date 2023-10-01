/*
 * m8ec - Embedded Client for the Dirtywave M8 Headless device.
 * Copyright (C) 2023 Lukas Neverauskis https://github.com/lukasnee
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "m8ec/font.hpp"

namespace m8ec::font {

ili9341_font_t const trash80_stealth57 = {
    .width = 5,
    .height = 7,
    .spacing = 1,
    .glyph =
        {
            0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, //
            0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x0000, 0x8000, //  !
            0xa000, 0xa000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, //  "
            0x0000, 0x5000, 0xf800, 0x5000, 0xf800, 0x5000, 0x0000, //  #
            0x2000, 0xf800, 0xa000, 0xf800, 0x2800, 0xf800, 0x2000, //  $
            0x0000, 0xc800, 0xd000, 0x2000, 0x5800, 0x9800, 0x0000, //  %
            0x6000, 0x9000, 0xa000, 0x4800, 0xa800, 0x9000, 0x6800, //  &
            0x8000, 0x8000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, //  '
            0x4000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x4000, //  (
            0x8000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x8000, //  )
            0x0000, 0xa000, 0x4000, 0xa000, 0x0000, 0x0000, 0x0000, //  *
            0x0000, 0x0000, 0x4000, 0xe000, 0x4000, 0x0000, 0x0000, //  +
            0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x8000, 0x8000, //  ,
            0x0000, 0x0000, 0x0000, 0xf800, 0x0000, 0x0000, 0x0000, //  -
            0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x8000, //  .
            0x0800, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000, 0x8000, //  /
            0xf800, 0x8800, 0x9800, 0xa800, 0xc800, 0x8800, 0xf800, //  0
            0xe000, 0x2000, 0x2000, 0x2000, 0x2000, 0x2000, 0xf800, //  1
            0xf800, 0x0800, 0x0800, 0xf800, 0x8000, 0x8000, 0xf800, //  2
            0xf800, 0x0800, 0x0800, 0xf800, 0x0800, 0x0800, 0xf800, //  3
            0x8800, 0x8800, 0x8800, 0xf800, 0x0800, 0x0800, 0x0800, //  4
            0xf800, 0x8000, 0x8000, 0xf800, 0x0800, 0x0800, 0xf800, //  5
            0xf800, 0x8000, 0x8000, 0xf800, 0x8800, 0x8800, 0xf800, //  6
            0xf800, 0x0800, 0x0800, 0x1000, 0x2000, 0x2000, 0x2000, //  7
            0x7800, 0x8800, 0x8800, 0x7000, 0x8800, 0x8800, 0xf000, //  8
            0xf800, 0x8800, 0x8800, 0xf800, 0x0800, 0x0800, 0x0800, //  9
            0x0000, 0x0000, 0x8000, 0x0000, 0x0000, 0x8000, 0x0000, //  :
            0x0000, 0x0000, 0x8000, 0x0000, 0x0000, 0x8000, 0x8000, //  ;
            0x0000, 0x2000, 0x4000, 0x8000, 0x4000, 0x2000, 0x0000, //  <
            0x0000, 0x0000, 0xe000, 0x0000, 0xe000, 0x0000, 0x0000, //  =
            0x0000, 0x8000, 0x4000, 0x2000, 0x4000, 0x8000, 0x0000, //  >
            0x7000, 0x8800, 0x0800, 0x1000, 0x2000, 0x0000, 0x2000, //  ?
            0x7000, 0x8800, 0xb800, 0xa800, 0xb800, 0x8000, 0x7000, //  @
            0x7000, 0x8800, 0x8800, 0xf800, 0x8800, 0x8800, 0x8800, //  A
            0xf000, 0x8800, 0x8800, 0xf000, 0x8800, 0x8800, 0xf000, //  B
            0x7000, 0x8800, 0x8000, 0x8000, 0x8000, 0x8800, 0x7000, //  C
            0xf000, 0x8800, 0x8800, 0x8800, 0x8800, 0x8800, 0xf000, //  D
            0xf800, 0x8000, 0x8000, 0xf000, 0x8000, 0x8000, 0xf800, //  E
            0xf800, 0x8000, 0x8000, 0xf000, 0x8000, 0x8000, 0x8000, //  F
            0x7000, 0x8800, 0x8000, 0x8000, 0x9800, 0x8800, 0x7000, //  G
            0x8800, 0x8800, 0x8800, 0xf800, 0x8800, 0x8800, 0x8800, //  H
            0xf800, 0x2000, 0x2000, 0x2000, 0x2000, 0x2000, 0xf800, //  I
            0x0800, 0x0800, 0x0800, 0x0800, 0x8800, 0x8800, 0x7000, //  J
            0x8800, 0x9000, 0xa000, 0xc000, 0xa000, 0x9000, 0x8800, //  K
            0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0xf800, //  L
            0x8800, 0xd800, 0xa800, 0x8800, 0x8800, 0x8800, 0x8800, //  M
            0x8800, 0xc800, 0xa800, 0x9800, 0x8800, 0x8800, 0x8800, //  N
            0x7000, 0x8800, 0x8800, 0x8800, 0x8800, 0x8800, 0x7000, //  O
            0xf000, 0x8800, 0x8800, 0xf000, 0x8000, 0x8000, 0x8000, //  P
            0x7000, 0x8800, 0x8800, 0x8800, 0xa800, 0x9000, 0x6800, //  Q
            0xf000, 0x8800, 0x8800, 0xf000, 0x9000, 0x8800, 0x8800, //  R
            0x7800, 0x8000, 0x8000, 0x7000, 0x0800, 0x0800, 0xf000, //  S
            0xf800, 0x2000, 0x2000, 0x2000, 0x2000, 0x2000, 0x2000, //  T
            0x8800, 0x8800, 0x8800, 0x8800, 0x8800, 0x8800, 0x7000, //  U
            0x8800, 0x8800, 0x8800, 0x8800, 0x5000, 0x5000, 0x2000, //  V
            0x8800, 0x8800, 0x8800, 0x8800, 0xa800, 0xd800, 0x8800, //  W
            0x8800, 0x8800, 0x5000, 0x2000, 0x5000, 0x8800, 0x8800, //  X
            0x8800, 0x8800, 0x8800, 0x7800, 0x0800, 0x0800, 0xf000, //  Y
            0xf800, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000, 0xf800, //  Z
            0xc000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0xc000, //  [
            0x8000, 0x8000, 0x4000, 0x2000, 0x1000, 0x0800, 0x0800, /* \ */
            0xc000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0xc000, //  ]
            0x4000, 0xa000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, //  ^
            0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xf800, //  _
            0x8000, 0x8000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, //  `
            0x0000, 0x0000, 0x7800, 0x8800, 0x8800, 0x8800, 0x7800, //  a
            0x0000, 0x8000, 0xf000, 0x8800, 0x8800, 0x8800, 0xf000, //  b
            0x0000, 0x0000, 0x7800, 0x8000, 0x8000, 0x8000, 0x7800, //  c
            0x0000, 0x0800, 0x7800, 0x8800, 0x8800, 0x8800, 0x7800, //  d
            0x0000, 0x0000, 0x7000, 0x8800, 0xf800, 0x8000, 0x7800, //  e
            0x0000, 0x3800, 0x4000, 0x4000, 0xf800, 0x4000, 0x4000, //  f
            0x0000, 0x0000, 0xf800, 0x8800, 0xf800, 0x0800, 0x7000, //  g
            0x0000, 0x0000, 0x8000, 0xf000, 0x8800, 0x8800, 0x8800, //  h
            0x4000, 0x0000, 0x4000, 0x4000, 0x4000, 0x4000, 0xe000, //  i
            0x0000, 0x0000, 0x0800, 0x0800, 0x0800, 0x0800, 0xf000, //  j
            0x0000, 0x0000, 0x8800, 0x9000, 0xe000, 0x9000, 0x8800, //  k
            0x0000, 0x0000, 0x8000, 0x8000, 0x8000, 0x8000, 0x7800, //  l
            0x0000, 0x0000, 0xd800, 0xa800, 0xa800, 0xa800, 0xa800, //  m
            0x0000, 0x0000, 0xf000, 0x8800, 0x8800, 0x8800, 0x8800, //  n
            0x0000, 0x0000, 0x7000, 0x8800, 0x8800, 0x8800, 0x7000, //  o
            0x0000, 0x0000, 0xf800, 0x8800, 0x8800, 0xf000, 0x8000, //  p
            0x0000, 0x0000, 0x7800, 0x8800, 0x8800, 0xf800, 0x0800, //  q
            0x0000, 0x0000, 0x7800, 0x8000, 0x8000, 0x8000, 0x8000, //  r
            0x0000, 0x0000, 0x7800, 0x8000, 0xf800, 0x0800, 0xf000, //  s
            0x0000, 0x4000, 0xf800, 0x4000, 0x4000, 0x4000, 0x3800, //  t
            0x0000, 0x0000, 0x8800, 0x8800, 0x8800, 0x8800, 0x7000, //  u
            0x0000, 0x0000, 0x8800, 0x8800, 0x8800, 0x5000, 0x2000, //  v
            0x0000, 0x0000, 0xa800, 0xa800, 0xa800, 0xa800, 0x5000, //  w
            0x0000, 0x0000, 0x8800, 0x5000, 0x2000, 0x5000, 0x8800, //  x
            0x0000, 0x0000, 0x8800, 0x8800, 0x7800, 0x0800, 0xf000, //  y
            0x0000, 0x0000, 0xf800, 0x1000, 0x2000, 0x4000, 0xf800, //  z
            0x4000, 0x8000, 0x8000, 0x4000, 0x8000, 0x8000, 0x4000, //  {
            0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, //  |
            0x8000, 0x4000, 0x4000, 0x8000, 0x4000, 0x4000, 0x8000, //  }
            0x0000, 0x4800, 0xb000, 0x0000, 0x0000, 0x0000, 0x0000, //  ~
        },
};

} // namespace m8ec::font