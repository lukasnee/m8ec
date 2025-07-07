#pragma once

#include "usbh_core.h"
#include "usbh_audio.h"

namespace m8ec::periph {

class UsbAudio {

public:
    UsbAudio() = default;

    bool init(struct usbh_audio *audio_class);
};

} // namespace m8ec::periph
