# Ay38910a Synth

## Pre-req

### Debian-like

```bash
sudo apt install gcc-avr gdb-avr avrdude avr-libc # debian-like
```

### Mingw64 over msys2
```bash
pacman -Ss mingw-w64-x86_64-avr-binutils \
           mingw-w64-x86_64-avr-gcc \
           mingw-w64-x86_64-avr-gdb \
           mingw-w64-x86_64-avr-libc \
           mingw-w64-x86_64-avrdude
```

# Build

Create a json file called ```conf.json``` in the root directory of the project, with the following contents:

```json
{
  "serial_port": "..."
}
```

and specify the serial port where to find the device to program.
If you attempt to configure the cmake build, 
a stub for the conf.json file will be created automatically.

Once you have the file in place, you can initialize the cmake project and start building/flashing.

```bash
mkdir build && cd build
cmake .. -B .
make
make flash
```
