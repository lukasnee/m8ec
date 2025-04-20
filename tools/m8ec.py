import os
import subprocess
import argparse
import sys
import logging
log = logging.getLogger("m8ec")

comm_relpath = os.path.normpath(os.path.join(os.path.dirname(__file__), "..",
                                             "extern", "W25Q64_STM32H750VB-DevEBox", "tools", "comm"))
if not os.path.exists(comm_relpath):
    raise Exception(f"comm does exist at {comm_relpath}")
# autopep8: off
sys.path.append(comm_relpath)
from comm import Comm
# autopep8: on


def sys_cmd(cmd):
    print(' '.join(cmd))
    try:
        if subprocess.call(cmd) != 0:
            raise Exception(f"Command failed: {' '.join(cmd)}")
    except KeyboardInterrupt:
        pass


def reset_target():
    sys_cmd(["openocd", "-f", "openocd.jlink.cfg",
            "-c", "init;reset;shutdown"])


def main():
    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers(dest='command', help='Sub-command help')

    upload_parser = subparsers.add_parser('upload', help='Upload a file')
    upload_parser.add_argument(
        'src_path', type=str, help='Source file path (host)')
    upload_parser.add_argument(
        'dst_path', type=str, help='Destination file path (target)')
    parser.add_argument(
        "-f", "--flash", help="flash (built) firmware to device", action="store_true")
    parser.add_argument("-r", "--reset", help="reset board",
                        action="store_true")
    parser.add_argument("--serial", help="enable serial", action="store_true")
    parser.add_argument(
        "--serial_dev", help="serial device for flashing and potentially for interfacing application", default="/dev/ttyACM0")
    parser.add_argument("--serial_log", help="serial log path",
                        default="serial.log")
    parser.add_argument('-l', '--log_level', type=int,
                        default=logging.WARNING, help='Log level')
    parser.add_argument('-L', '--log_file', type=str,
                        default=None, help='Log file')

    args = parser.parse_args()

    logging.basicConfig(level=args.log_level, filename=args.log_file,
                        format='%(asctime)s|%(levelname)s|%(name)s|%(message)s')

    # flashing requires reset to enter bootloader
    if args.reset or args.flash:
        reset_target()

    if args.flash:
        comm = Comm(args.serial_dev, log_level=args.log_level)
        comm.capture_bootloader(10.0)
        comm.upload_file(".build/source/m8ec.bin", "boot/app.bin")
        comm.release_bootloader()

    if args.command == "upload":
        # with speedscope.track('speedscope.json'):
        comm = Comm(args.serial_dev, log_level=args.log_level)
        comm.capture_bootloader(10.0)
        comm.upload_file(args.src_path, args.dst_path)
        comm.release_bootloader()

    if args.serial:
        sys_cmd(["minicom", "--baudrate", "921600", "-D",
                args.serial_dev, "-C", args.serial_log])


if __name__ == "__main__":
    main()
