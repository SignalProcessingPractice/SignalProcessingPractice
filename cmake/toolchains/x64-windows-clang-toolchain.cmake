# CMake toolchain file

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(CMAKE_C_COMPILER clang CACHE STRING "C compiler" FORCE)
set(CMAKE_CXX_COMPILER clang++ CACHE STRING "C++ compiler" FORCE)

# コンパイラオプションを設定
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

# Native ビルド用の CMSIS-DSP ビルドオプションを設定
# 参考: https://github.com/ARM-software/CMSIS-DSP#how-to-install-as-a-cmake-package
set(HOST ON CACHE BOOL "")
set(DISABLEFLOAT16 ON CACHE BOOL "")
