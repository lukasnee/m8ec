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

#include <cstdint>

namespace m8ec {

class IKeys {

public:
    enum Key : uint8_t {
        edit,
        option,
        right,
        play,
        shift,
        down,
        up,
        left,

        none = 0xff,
    };

    static constexpr Key keys[] = {Key::edit, Key::option, Key::right, Key::play, Key::shift, Key::down, Key::up, Key::left};

    static const char *key_to_string(Key key) {
        switch (key) {
        case Key::edit:
            return "edit";
        case Key::option:
            return "option";
        case Key::right:
            return "right";
        case Key::play:
            return "play";
        case Key::shift:
            return "shift";
        case Key::down:
            return "down";
        case Key::up:
            return "up";
        case Key::left:
            return "left";
        default:
            return "none";
        }
    }

    struct State {
        void set(Key key, bool value) {
            if (value) {
                this->underlying |= 1 << key;
            }
            else {
                this->underlying &= ~(1 << key);
            }
        }

        bool get(Key key) const { return this->underlying & (1 << key); }

        std::uint8_t underlying;
    };

protected:
    virtual bool init() = 0;
    virtual State getState() = 0;

private:
};

} // namespace m8ec
