# m8ec

m8ec is a power-efficient and cost-efficient embedded peripheral system for the
[Headless Dirtywave M8](https://github.com/Dirtywave/M8HeadlessFirmware).
Currently, the project is in the prototype stage.

## Goal

Complement the Dirtywave M8 headless (Teensy 4.1) with a display, keypad, and an
audio interface that are similar to the original M8 hardware unit. The
peripherals may be driven by a microcontroller acting as a "client" (USB host in
fact) to the Teensy 4.1 running M8 headless firmware. The hardware peripherals
should be easily available and inexpensive.

Also, the goal is to have fun and learn something along the way.

```mermaid
graph LR
    D[Keypad] --> B
    A[Dirtywave M8 Headless] -----|USB| B[STM32 MCU]
    B --> C[Display]
    B --> E[Audio Out]
    F[Audio In] --> B
    subgraph m8esc
        B
        C
        D
        E
        F
    end
```

## Prototype Hardware

- MCU dev board:
  [lukasnee/W25Q64_STM32H750VB-DevEBox](https://github.com/lukasnee/W25Q64_STM32H750VB-DevEBox.git)
- Display: 2.8" TFT LCD (ILI9341) module.
- Keypad: Modified USB mechanical key number pad as GPIO input. Probably...
- Audio out: PCM5102A module.
- Audio in: PCM1802 module.

## Development Environment Setup

This project was originally developed in WSL Ubuntu, and still is the primary
development environment.

1. Install these prerequisites:

    ```bash
    sudo apt update && sudo apt upgrade -y
    sudo apt install -y git ninja-build python3
    ```

    `sudo apt install -y cmake` may install older version of CMake than 4.0.0.
    Instead you may want to install latest CMake from
    [here](https://cmake.org/download/). Look for
    `cmake-<VERSION>-linux-x86_64.sh`.

    Install CMake:

    ```bash
    cd /opt/
    sh cmake-<VERSION>-linux-x86_64.sh # agree with creating directory (y)
    echo 'export PATH=$PATH:/opt/cmake-<VERSION>-linux-x86_64/bin' >> ~/.bashrc
    source ~/.bashrc
    ```

2. Install Arm GNU Toolchain:

    > for more details see
    > [this](https://lindevs.com/install-arm-gnu-toolchain-on-ubuntu)  

    ```bash
    ARM_TOOLCHAIN_VERSION=$(curl -s https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads | grep -Po '<h4>Version \K.+(?=</h4>)')
    curl -Lo gcc-arm-none-eabi.tar.xz "https://developer.arm.com/-/media/Files/downloads/gnu/${ARM_TOOLCHAIN_VERSION}/binrel/arm-gnu-toolchain-${ARM_TOOLCHAIN_VERSION}-x86_64-arm-none-eabi.tar.xz"
    sudo mkdir /opt/gcc-arm-none-eabi
    sudo tar xf gcc-arm-none-eabi.tar.xz --strip-components=1 -C /opt/gcc-arm-none-eabi
    echo 'export PATH=$PATH:/opt/gcc-arm-none-eabi/bin' | sudo tee -a /etc/profile.d/gcc-arm-none-eabi.sh
    source /etc/profile
    arm-none-eabi-gcc --version
    arm-none-eabi-g++ --version
    arm-none-eabi-gdb --version
    rm -rf gcc-arm-none-eabi.tar.xz
    ```

    If getting `arm-none-eabi-gdb: error while loading shared libraries:
    libncursesw.so.5` error, install `libncurses5`:

    ```bash
    sudo apt install -y libncurses5
    ```

    If that does not help, try this:

    ```bash
    sudo apt install -y gdb-multiarch
    sudo mv /usr/bin/arm-none-eabi-gdb /usr/bin/arm-none-eabi-gdb.bak
    sudo ln -s /usr/bin/gdb-multiarch /usr/bin/arm-none-eabi-gdb
    ```

### Install J-Link Software and Documentation Pack

1. Go to [SEGGER J-Link Software and Documentation Pack](https://www.segger.com/downloads/jlink/#J-LinkSoftwareAndDocumentationPack)

2. Download `64-bit DEB Installer`.

3. Install the downloaded package:

    ```bash
    sudo dpkg -i JLink_Linux_V<XXX>_x86_64.deb
    echo 'export PATH=/opt/SEGGER/JLink:$PATH' >> ~/.bashrc
    source ~/.bashrc
    ```

## Building the Firmware

1. Clone the project and its submodules:

    ```bash
    git clone https://github.com/lukasnee/m8ec.git
    git checkout prototype
    git submodule update --init --recursive
    ```

2. TODO: instruction for building and flashing the bootloader
   [bl_iram](extern/W25Q64_STM32H750VB-DevEBox/docs/bl_iram.md)
   for STM32H750 (DevEBox) platform.

3. Build using the project tool:

    ```bash
    cmake --workflow STM32H750-rel
    ```

4. Build debug version:

    ```bash
    cmake --workflow STM32H750-dbg
    ```

> Run `python3 tools/m8ec.py -h` to see more options.

## Flashing

The firmware variant is based on
[lukasnee/W25Q64_STM32H750VB-DevEBox](https://github.com/lukasnee/W25Q64_STM32H750VB-DevEBox.git)
project. It has a [bl_iram](extern//W25Q64_STM32H750VB-DevEBox/docs/bl_iram.md)
bootloader with a file system mounted on the external QSPI FLASH. The features a
communication protocol via serial for transferring application binary to the
device. The bootloader will then load the firmware to RAM and execute it.

The `tools/m8ec.py` has the bootloader communication protocol integrated, so the flashing is easy.

```bash
python3 tools/m8ec.py -f
```

> If using WSL, make sure to attach the ST-Link USB interface. There is a very
> convenient VSCode extension to do that effortlessly
> `thecreativedodo.usbip-connect`.

## Debugging

### Serial (printf)

1. Install `minicom`.

    ```bash
    sudo apt-get install -y minicom
    ```

2. Open a terminal using the project tool.

    ```bash
    python3 tools/m8ec.py --serial
    ```

### SEGGER SystemView + ST-Link as J-Link

> Here we use SystemView GUI for Windows, but it supports other OSes as well.

1. Build firmware with SEGGER SystemView enabled.

    ```bash
    cmake --workflow STM32H750-sysview
    ```

2. Flash the firmware to the target MCU.

    ```bash
    python3 tools/m8ec.py -f
    ```

3. Install latest [SEGGER
   SystemView](https://www.segger.com/products/development-tools/systemview/)
   and J-Link drivers.
4. [STLinkReflash](https://www.segger.com/products/debug-probes/j-link/models/other-j-links/st-link-on-board/)
   tool can be used to reflash the ST-Link firmware to J-Link and back (ST-Link
   V2-1 only).

> In case [J-Link shown as generic BULK device in
> Windows](https://wiki.segger.com/J-Link_shown_as_generic_BULK_device_in_Windows).

5. Start SystemView and connect to the target MCU.

6. TBD...

## TODO

- Consider switching to
  [jonenz/FreeRTOS-Cpp](https://github.com/jonenz/FreeRTOS-Cpp) instead of
  [freertos-addons](https://github.com/michaelbecker/freertos-addons). It is
  newer and more actively maintained, comes with CMake support, more modern C++,
  more familiar and arguably better project structure, testing and CI.
- Consider upgrading to [this board](https://www.aliexpress.com/item/1005007227916481.html?spm=a2g0o.productlist.main.5.240f5dfbcMZCOX&algo_pvid=97c56860-6bd8-4b73-bccb-8bab31fc1a4f&algo_exp_id=97c56860-6bd8-4b73-bccb-8bab31fc1a4f-2&pdp_ext_f=%7B%22order%22%3A%2254%22%2C%22eval%22%3A%221%22%7D&pdp_npi=4%40dis%21EUR%2123.88%2121.49%21%21%21177.55%21159.78%21%40211b80e117398267511244870e7eaf%2112000039882353598%21sea%21LT%210%21ABX&curPageLogUid=Q83YcP7qsL2k&utparam-url=scene%3Asearch%7Cquery_from%3A) - 2MB flash and a 7" IPS display.
- Figure out Lua dynamic memory allocation and how to port it to FreeRTOS.
- Worthy reads:
  - [From Zero to main(): Bootstrapping libc with Newlib](https://interrupt.memfault.com/blog/boostrapping-libc-with-newlib)
- looks like the ucprof trims a lot recording from start and end. Figure out whats up.

- Figure out the USB client interface. Research what M8 Headless expects
  from the client.
  - <https://github.com/laamaa/m8c> could be very useful, although it's for
    Windows/Linux/MacOS.
- Get the USB client working with the MCU dev board.
- Get the display working with the M8 headless via USB client.
- Modify the keypad to be driven by the MCU dev board GPIO.
- Get The Keypad working with the MCU dev board.
- Get the keypad working with the M8 headless via USB client.
- Hook up the audio-out interface to the MCU dev board.
- Get the audio out working with the MCU dev board.
- Get the audio out working with the M8 headless via USB client.
- Hook up the audio in the interface to the MCU dev board.
- Get the audio in working with the MCU dev board.
- Try to get the audio working with the M8 headless via a USB client. From
  reading online, it's tricky.

## Future Ideas

- [ ] MIDI in/out? If supported by the M8 headless.
- [ ] Eurorack System Integration? Maybe make a module out of it.

## Links

- Original Dirtywave M8: <https://dirtywave.com/products/m8-tracker>
- M8 Headless: <https://github.com/Dirtywave/M8HeadlessFirmware>
