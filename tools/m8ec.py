import os
import sys
import argparse
import subprocess

parser = argparse.ArgumentParser()
parser.add_argument("-b", "--build", help="build project", action="store_true")
parser.add_argument("-f", "--flash", help="flash project", action="store_true")
parser.add_argument("-r", "--reset", help="reset board", action="store_true")
args = parser.parse_args()

os.chdir("CubeMX")

if args.build:
    print("Building project")
    error = subprocess.call(["make"])
    
if args.flash:
    if args.build and error != 0:
        print("Error: build failed skipping flash")
        sys.exit(1) 
    print("Flashing project")
    subprocess.call(["st-flash", "--format", "ihex", "write", "build/CubeMX.hex"])

if args.reset:
    print("Resetting board")
    subprocess.call(["st-flash", "reset"])