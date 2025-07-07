#include "m8ec/periph/UsbAudio.hpp"

#include "fonas/logger/logger.hpp"

LOG_MODULE(m8ec::periph::UsbAudio, LOGGER_LEVEL_INFO);

namespace m8ec::periph {

bool UsbAudio::init(struct usbh_audio *audio_class) {
    (void)audio_class;

    // Initialization code for USB Audio
    LOG_INFO("Initializing USB Audio");
    // Here you would typically set up the USB audio class, endpoints, etc.
    return true; // Return true if initialization is successful
}

bool tx_flag = 0;
bool rx_flag = 0;

extern "C" void usbd_audio_open(uint8_t busid, uint8_t intf) {
    (void)busid;
    if (intf == 1) {
        rx_flag = 1;
        // /* setup first out ep read transfer */
        // usbd_ep_start_read(busid, AUDIO_OUT_EP, read_buffer, AUDIO_OUT_PACKET);
        LOG_INFO("OPEN1");
    }
    else {
        tx_flag = 1;
        LOG_INFO("OPEN2");
    }
}

extern "C" void usbd_audio_close(uint8_t busid, uint8_t intf) {
    (void)busid;
    if (intf == 1) {
        rx_flag = 0;
        LOG_INFO("CLOSE1");
    }
    else {
        tx_flag = 0;
        LOG_INFO("CLOSE2");
    }
}

} // namespace m8ec::periph