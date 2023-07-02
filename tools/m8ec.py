import sys
import argparse
import subprocess

parser = argparse.ArgumentParser()
parser.add_argument("-d", "--debug", help="enable debug", action="store_true")
parser.add_argument("-b", "--build", help="build project", action="store_true")
parser.add_argument("-f", "--flash", help="flash project", action="store_true")
parser.add_argument("-r", "--reset", help="reset board", action="store_true")
args = parser.parse_args()

BUILD_DIR = ".build"

if args.build:
    print("Building project")
    error = subprocess.call(["cmake", "-S", ".", "-B", BUILD_DIR, "-DDEBUG={}".format("ON" if args.debug else "OFF"), "-G", "Unix Makefiles", '-DCMAKE_TOOLCHAIN_FILE=arm-none-eabi-gcc.cmake', "-DCMAKE_BUILD_TYPE=release"])
    if error != 0:
        print("Error: cmake failed")
        sys.exit(1)
    error = subprocess.call(["cmake", "--build", BUILD_DIR, "--", "-j4"])
    if error != 0:
        print("Error: build failed")
        sys.exit(1)
    
if args.flash:
    if args.build and error != 0:
        print("Error: build failed skipping flash")
        sys.exit(1) 
    print("Flashing project")
    subprocess.call(["st-flash", "--format", "ihex", "write", ".build/CubeMX/firmware.hex"])

if args.reset:
    print("Resetting board")
    subprocess.call(["st-flash", "reset"])