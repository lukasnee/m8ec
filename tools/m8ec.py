import os
import subprocess
import argparse
import sys
import logging
# import speedscope

log = logging.getLogger("m8ec")

comm_relpath = os.path.normpath(os.path.join(os.path.dirname(__file__), "..",
                                             "extern", "W25Q64_STM32H750VB-DevEBox", "tools", "comm"))
if not os.path.exists(comm_relpath):
    raise Exception(f"comm does exist at {comm_relpath}")
# autopep8: off
sys.path.append(comm_relpath)
from comm import Comm
import time
# autopep8: on


def sys_cmd(cmd):
    print(' '.join(cmd))
    try:
        if subprocess.call(cmd) != 0:
            raise Exception(f"Command failed: {' '.join(cmd)}")
    except KeyboardInterrupt:
        pass


def reset_target():
    sys_cmd(["openocd", "-f", "config/openocd.jlink.cfg",
            "-c", "init;reset;shutdown"])


def main():
    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers(dest='command', help='Sub-command help')

    upload_parser = subparsers.add_parser('upload', help='Upload a file')
    upload_parser.add_argument(
        'src_path', type=str, help='Source file path (host)')
    upload_parser.add_argument(
        'dst_path', type=str, help='Destination file path (target)')
    # parser.add_argument(
    #     "--build_dir", help="build directory", default=".build")
    parser.add_argument(
        "-f", "--flash", help="flash firmware to device", action="store_true")
    parser.add_argument(
        "--fw_path", help="firmware path", default=".build/source/m8ec.bin")
    parser.add_argument("-r", "--reset", help="reset board",
                        action="store_true")
    # parser.add_argument("-p", "--platform", help="platform",
    #                     choices=["STM32H750", "STM32F411"], default="STM32H750")
    parser.add_argument("--serial", help="enable serial", action="store_true")
    parser.add_argument(
        "--serial_dev", help="serial device for flashing and potentially for interfacing application", default="/dev/ttyACM0")
    parser.add_argument("--serial_log", help="serial log path",
                        default="m8ec.log")
    parser.add_argument('-l', '--log_level', type=int,
                        default=logging.WARNING, help='Log level')
    parser.add_argument('-L', '--log_file', type=str,
                        default=None, help='Log file')
    # parser.add_argument(
    #     "--ucprof", help="ucprof path. Record profiling data", default=None)

    args = parser.parse_args()

    logging.basicConfig(level=args.log_level, filename=args.log_file,
                        format='%(asctime)s|%(levelname)s|%(name)s|%(message)s')

    # flashing requires reset to enter bootloader
    if args.reset or args.flash:
        reset_target()
        if args.flash:
            time.sleep(0.1)

    if args.flash:
        comm = Comm(args.serial_dev, log_level=args.log_level)
        comm.capture_bootloader(10.0)
        comm.upload_file(args.fw_path, "boot/app.bin")
        comm.release_bootloader()

    if args.command == "upload":
        # with speedscope.track('speedscope.json'):
        comm = Comm(args.serial_dev, log_level=args.log_level)
        comm.capture_bootloader(10.0)
        comm.upload_file(args.src_path, args.dst_path)
        comm.release_bootloader()

    # if args.ucprof:
        # sudo /mnt/c/Program\ Files/SEGGER/JLink_V794b/JLinkRTTLogger.exe -Device STM32H750VB -If SWD -Speed 4000 -RTTChannel 2 ucprof.dat
        # arm-none-eabi-nm -lnC .build/source/m8ec > .build/source/m8ec.symbols
        # python3 ucprof.py .build/source/m8ec.symbols ucprof.dat

        # sys_cmd([
        #     "sudo",
        #     "/mnt/c/Program Files/SEGGER/JLink_V794b/JLinkRTTLogger.exe",
        #     "-Device",
        #     "STM32H750VB",
        #     "-If",
        #     "SWD",
        #     "-Speed",
        #     "4000",
        #     "-RTTChannel",
        #     "2",
        #     args.ucprof])
        # sys_cmd([
        #     "arm-none-eabi-nm",
        #     "-lnC",
        #     f"{args.build_dir}/platform/{args.platform}/{args.platform}",
        #     ">",
        #     f"{args.build_dir}/platform/{args.platform}/{args.platform}.symbols"])
        # sys_cmd([
        #     "python3",
        #     "extern/ucprof/ucprof.py",
        #     f"{args.build_dir}/platform/{args.platform}/{args.platform}.symbols",
        #     args.ucprof])

    if args.serial:
        sys_cmd(["minicom", "--baudrate", "921600", "-D",
                args.serial_dev, "-C", args.serial_log, "--color=off"])


if __name__ == "__main__":
    main()
