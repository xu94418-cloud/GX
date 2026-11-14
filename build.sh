#!/usr/bin/env bash
set -euo pipefail
cd -- "$(dirname -- "$0")"
: "${LD64_LLD:?Set LD64_LLD to an ld64.lld executable}"
python3 -m ziglang c++ -target aarch64-ios.13.0 -c -O2 -fno-exceptions -fno-rtti -fno-stack-protector -fvisibility=hidden -Wall -Wextra -Werror JJFour.cpp -o JJFour.o
"$LD64_LLD" -dylib -arch arm64 -platform_version ios 13.0 16.0 -Lsdk -lSystem -undefined error -install_name @rpath/JJFour.dylib JJFour.o -o JJFour.dylib
