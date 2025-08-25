
# Development Information

This document provides information on m8ec development.

## Hardware

Currently there is only a single prototype hardware version. It consists of:

- MCU dev board:
  [lukasnee/STM32H7-DevEBox](https://github.com/lukasnee/STM32H7-DevEBox.git)
- Display: 2.8" TFT LCD (ILI9341) module via SPI interface.
- Keypad: hand-built mechanical key matrix driven directly via MCU GPIO pins.
- Audio out: PCM5102A module (not yet implemented).
- Audio in: PCM1802 module (not yet implemented).
- Development instrumentation: STM32 Nucleo board for ST-Link SWD + integrated
  serial interface.

> TODO: Add schematic.

## Firmware

This project is primarily developed on WSL Ubuntu (Linux). The
instructions below only cover setting up for that environment.

The m8ec application firmware is based on project
[lukasnee/STM32H7-DevEBox](https://github.com/lukasnee/STM32H7-DevEBox.git) and
requires the [`bl_iram`](../extern/STM32H7-DevEBox/docs/bl_iram.md) bootloader
to be flashed to the MCU first. `bl_iram` is a bootloader that loads application
firmware into MCU's internal RAM and runs it from there.

1. First, clone the m8ec project and its submodules:

    ```bash
    git clone https://github.com/lukasnee/m8ec.git
    git submodule update --init --recursive
    ```

2. Follow the [STM32H7-DevEBox Development Environment
Setup](../extern/STM32H7-DevEBox/docs/dev_env.md). It applies to m8ec as well.

3. Build and flash the `bl_iram` bootloader by following the [STM32H7-DevEBox
Development Guide](../extern/STM32H7-DevEBox/DEVELOPMENT.md).
 
4. Once that is done, you can build the m8ec application firmware:

    ```bash
    cmake --workflow STM32H750-rel # release build
    cmake --workflow STM32H750-dbg # debug build
    ```

5. And upload it to the MCU's file system:

    ```bash
    extern/STM32H7-DevEBox/.venv/bin/python3 tools/m8ec.py -f
    ```

> [!Note]
>
> If using WSL, make sure to attach the ST-Link USB interface. There is a very
> convenient VSCode extension to make it easier: `thecreativedodo.usbip-connect`.

## Debugging

### Serial (printf)

1. Install `minicom`.

    ```bash
    sudo apt-get install -y minicom
    ```

2. Open a terminal using the project tool.

    ```bash
    extern/STM32H7-DevEBox/.venv/bin/python3 tools/m8ec.py --serial
    ```

### SEGGER SystemView + ST-Link as J-Link

> Here we use SystemView GUI for Windows, but it supports other OSes as well.

1. Build firmware with SEGGER SystemView enabled.

    ```bash
    cmake --workflow STM32H750-sysview
    ```

2. Flash the firmware to the target MCU.

    ```bash
    extern/STM32H7-DevEBox/.venv/bin/python3 tools/m8ec.py -f
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

#### TODO

- Looks like the ucprof trims a lot of recording from start and end. Figure out what's up.
