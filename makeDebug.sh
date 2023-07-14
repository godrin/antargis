#!/bin/bash
#cmake --debug-output --trace -DCMAKE_BUILD_TYPE=Debug ..
#cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles" ..
export CC=/usr/bin/clang
export CXX=/usr/bin/clang++
# /usr/bin/../lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13
# /usr/bin/../lib/gcc/x86_64-linux-gnu/13/../../../../include/c++/13/backward
# /usr/lib/llvm-15/lib/clang/15.0.7/include
# /usr/local/include
# /usr/include/x86_64-linux-gnu
# /usr/include

cmake -Bbuild -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DLLVM_ENABLE_PROJECTS=clang
#make -j4 -C build
