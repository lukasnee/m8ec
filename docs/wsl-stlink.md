# Accessing [ST-Link/V2](https://www.st.com/en/development-tools/st-link-v2.html) USB Device in WSL

## Prerequisites

-   Windows 10
-   [WSL 2](https://learn.microsoft.com/en-us/windows/wsl/install) and Ubuntu (default distribution).
-   [USBIPD-Win](https://github.com/dorssel/usbipd-win/wiki/WSL-support).

## Instructions

1. Install ST-Link tools on Ubuntu.

    ```bash
    sudo apt-get install -y stlink-tools
    ```

1. Copy `stlinkv2.rules` to `/etc/udev/rules.d/` and restart udev.

    ```bash
    sudo cp stlinkv2.rules /etc/udev/rules.d/
    sudo service udev restart
    sudo udevadm control --reload
    ```

1. Connnect (or reconnect) ST-Link to PC.

1. Attach USB device to WSL (from Windows). Example:

    ```powershell
    usbipd wsl attach -a -i 0483:3748
    ```

1. Check if ST-Link is connected to WSL.

    ```bash
    lsusb
    ```

1. Run `st-info --probe` to check if ST-Link is connected.

Now you should be ready to use `tools/m8ec.py` to flash your board.
