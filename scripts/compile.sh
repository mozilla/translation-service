#!/bin/bash

set -e

mkdir -p build
cd build
cmake ../ -DUSE_WASM_COMPATIBLE_SOURCES=off -DCOMPILE_CPU=on -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)