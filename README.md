# Ay38910a Synth

# Pre-req

```bash
sudo apt install gcc-avr gdb-avr avrdude avr-libc 
```

# Build

```bash
cmake -S . -B ./cmake-build-debug-avr-gcc
cd cmake-build-debug-avr-gcc
make
make flash
```
