#!/usr/bin/env bash

targets=(atmega644 atmega2560)

if [ $# -ne 1 ]; then
  echo "setup-cmake expects one parameter."
  echo "Usage: python cmake-setup.py [target]"
  exit 1
fi

for target in "${targets[@]}"; do
  if [ "$target" == "$1" ]; then
    rm -rf build
    mkdir build
    cmake -S . -B build -DMCU="$target"
    exit 0
  fi
done

echo "Unsupported architecture, choose one from the following: ${targets[*]}"
exit 1