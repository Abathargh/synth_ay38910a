from serial.tools.list_ports import comports
from collections import OrderedDict
from typing import List

import serial
import struct
import sys
import re


shapes = OrderedDict([
    ("no envelope",  "___________________"),
    ("reverse sawtooth",  "\\|\\|\\|\\|\\|\\|\\|\\|\\|\\"),
    ("triangular oop",  "\\/\\/\\/\\/\\/\\/\\/\\/\\/\\"),
    ("up down const up",  "\\/‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾"),
    ("sawtooth",  "/|/|/|/|/|/|/|/|/|/"),
    ("down const up",  "/‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾"),
    ("triangular",  "/\\/\\/\\/\\/\\/\\/\\/\\/\\/"),
])


def usage():
    help_msg = """usage: ay38910a cli controller tool.
  - 'h', 'help':                show this message
  - 'q', 'quit':                exit the tool
  - 'amplitude, octave, shape': manage settings
      - 0 <= amplitude <= 15, 0 <= octave <= 8
      - for shape, use either its id or its string:"""
    longest = max([len(s) for s in shapes.keys()])
    for i, shape in enumerate(shapes):
        help_msg += f"\n{i: >9} - {shape: <{longest}} {shapes[shape]}"
    print(help_msg)


def validate_input(amp: str, octave: str, shape: str) -> (bool, str):
    amp = int(amp)
    octave = int(octave)
    try:
        shape = int(shape)
        shape_ok = 0 <= shape <= len(shapes)
    except ValueError:
        shape_ok = shape in shapes
        for i, s in enumerate(shapes):
            if s == shape:
                shape = i
                break

    return (0 <= amp <= 15) or (0 <= octave <= 8) or shape_ok, str(shape)


def list_devices() -> List[str]:
    ret = []
    for d in comports():
        ret.append(f"{d.device} ({d.manufacturer}, PID: {d.pid:04x}")
    return ret


def main():
    ok_req = re.compile(r"\s*[0-9]{1,2}\s*,\s*[0-9]{1,2}\s*,\s*[0-9a-zA-Z]+\s*")
    dev = None
    port = ""
    try:
        devs = list_devices()
        if len(devs) == 0:
            print("No device connected, exiting")
            sys.exit(0)

        print("Choose a device from the list, by typing its number:")
        while True:
            for i, d in enumerate(devs):
                print(f"  [{i}] {d}")

            try:
                dev_num = int(input(">>> "))
                if not (0 <= dev_num <= len(devs)):
                    print("Select a valid number")
                    continue
                port = devs[dev_num].split(" ")[0]
                break
            except ValueError:
                print("Select a valid number")

        dev = serial.Serial(port=port, baudrate=9600, bytesize=8, stopbits=1)
        print("Connected, input a frame, use 'h' or 'help' for more info")
        while True:
            req = input(">>> ").lower().strip()
            if req == "h" or req == "help":
                usage()
                continue
            if req == "q" or req == "quit":
                return
            if not ok_req.match(req):
                print("Invalid format, use 'h' or 'help' for more info")
                continue

            amp, octave, shape = req.split(",")
            amp = amp.strip()
            octave = octave.strip()
            shape = shape.strip()

            is_valid_input, shape = validate_input(amp, octave, shape)

            if not is_valid_input:
                print("Invalid format, use 'h' or 'help' for more info")
                sys.exit(1)

            amp = f"{amp:0<2}".encode("ascii")
            octave = octave.encode("ascii")
            shape = shape.encode("ascii")

            frame = struct.pack("<2sccc", amp, octave, shape, b'\n')
            dev.write(frame)
            ret = dev.readline()
            print(ret)
    except serial.SerialException as se:
        print(f"Serial exception: {se}")
    except KeyboardInterrupt:
        pass
    finally:
        print("Bye!")
        if dev is not None and dev.is_open:
            dev.close()


if __name__ == "__main__":
    main()
