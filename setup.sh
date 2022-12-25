#!/usr/bin/env bash

SERIAL_PORT=/dev/ttyACM0

targets=(atmega644 atmega2560)
prog_str=("atmelice" "stk500v2 -D -b 115200 -P $SERIAL_PORT")


for i in "${!targets[@]}"; do
  rm -r build-"${targets[$i]}"
  mkdir build-"${targets[$i]}"
  cmake -S . -B build-"${targets[$i]}" \
    -DMCU="${targets[$i]}" -DAVRDUDE_PRG_STR="${prog_str[$i]}"
done
