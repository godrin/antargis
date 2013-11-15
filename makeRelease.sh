#!/bin/bash
mkdir -p Release
cd Release
#cmake --debug-output --trace -DCMAKE_BUILD_TYPE=Debug ..
cmake -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" ..
make -j4
