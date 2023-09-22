import argparse
import subprocess
import threading

USBIPD_PATH = r'/mnt/c/Program\ Files/usbipd-win/usbipd.exe'

def run_command(command):
    subprocess.run(command, shell=True)

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--usb-id-stlink', type=str, default='0483:3748', help='USB ID of STLink')
    parser.add_argument('--usb-id-serial', type=str, default='10c4:ea60', help='USB ID of Serial Port')
    args = parser.parse_args()

    command1 = f'{USBIPD_PATH} wsl attach -a -i {args.usb_id_stlink}'
    command2 = f'{USBIPD_PATH} wsl attach -a -i {args.usb_id_serial}'

    thread1 = threading.Thread(target=run_command, args=(command1,))
    thread2 = threading.Thread(target=run_command, args=(command2,))

    thread1.start()
    thread2.start()

    thread1.join()
    thread2.join()
