#!/bin/bash
if [ ! -d "$1/assets/libtorch-cxx11-abi-shared-with-deps-2.0.1+cpu.zip" ]; then
  print_process "Downloading libtorch 2.0.1+cpu.zip.."
  wget https://download.pytorch.org/libtorch/cpu/libtorch-cxx11-abi-shared-with-deps-2.0.1%2Bcpu.zip -P $1/assets/
fi

unzip -o $1/assets/libtorch-cxx11-abi-shared-with-deps-2.0.1+cpu.zip -d $1/deps/