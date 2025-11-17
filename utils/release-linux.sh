#!/usr/bin/sh

rm -rf build
find deploy -name "*.zip" -delete
cmake -Bbuild . -GNinja -DCMAKE_BUILD_TYPE=Release "$@"
cmake --build build --config Release -j2
