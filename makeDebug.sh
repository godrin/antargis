#!/bin/bash
mkdir -p Debug
cd Debug
#cmake --debug-output --trace -DCMAKE_BUILD_TYPE=Debug ..
cmake -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles" ..
make -j4
