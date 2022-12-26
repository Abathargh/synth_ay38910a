# Ay38910a Synth

A synth based on the AY38910a programmable sound generator chip, implemented using an megaAVR ATMega microcontroller
(currently tested with an ATMega644 standalone system and on an Arduino Mega).

## Pre-reqs

### Debian-like

```bash
sudo apt install gcc-avr gdb-avr avrdude avr-libc
```

### Mingw64 over msys2
```bash
pacman -S mingw-w64-x86_64-avr-binutils \
          mingw-w64-x86_64-avr-gcc \
          mingw-w64-x86_64-avr-gdb \
          mingw-w64-x86_64-avr-libc \
          mingw-w64-x86_64-avrdude
```

## Build

```bash
mkdir build && cd build
cmake .. -B .

# explicitly pass the mcu and prog string
cmake .. -B . -DMCU=atmega644 -DAVRDUDE_PRG_STR=atmelice

make
make flash
make flash-debug

make docs
make clean-docs
```
