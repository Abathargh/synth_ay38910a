#!/usr/bin/env bash

set -x

targets=(atmega644 atmega2560)
release="-DCMAKE_BUILD_TYPE=Debug"

while getopts "r" opt; do
  case "$opt" in
    r)  release="-DCMAKE_BUILD_TYPE=Release";;
    \?) printf "illegal option: -%s\n" "$OPTARG" >&2
        exit 1 ;;
    :)  printf "missing argument for -%s\n" "$OPTARG" >&2
        exit 1;;
  esac
done


shift $(($OPTIND - 1))
passed_target=$1

for target in "${targets[@]}"; do
  if [ "$target" == "$passed_target" ]; then
    rm -rf build
    mkdir build
    cmake -S . -B build -DMCU="$target" "$release"
    exit 0
  fi
done

echo "Unsupported architecture, choose one from the following: ${targets[*]}"
exit 1