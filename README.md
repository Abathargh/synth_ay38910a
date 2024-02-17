# ay38910a synth

A synth based on the AY38910a programmable sound generator chip, implemented 
using an ATMega microcontroller (currently tested with an ATMega644 standalone 
system and on an Arduino Mega).

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
./scripts/setup.sh atmega644    # or ./scripts/setup.sh atmega2560
./scripts/setup.sh -r atmega644 # -r => release mode

# or 
mkdir build && cd build
cmake .. -B .

# explicitly pass the mcu and prog string
cmake .. -B . -DMCU=atmega644

make             # build hex/elf/bin
make flash       # flash the hex file
make flash-debug # flash the elf file

make docs
make clean-docs
```
