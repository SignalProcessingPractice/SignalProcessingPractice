# CMake toolchain file

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

#
# Qt のインストール先を確認
#
if(NOT DEFINED ENV{QT_ROOT_DIR})
    message(FATAL_ERROR "Environment variable QT_ROOT_DIR is not set.")
endif()
if(NOT DEFINED ENV{QT_TOOLCHAIN_ROOT})
    message(FATAL_ERROR "Environment variable QT_TOOLCHAIN_ROOT is not set.")
endif()

set(QT_ROOT_DIR "$ENV{QT_ROOT_DIR}")

set(CMAKE_C_COMPILER
    "$ENV{QT_TOOLCHAIN_ROOT}/bin/gcc.exe"
    CACHE FILEPATH "" FORCE)

set(CMAKE_CXX_COMPILER
    "$ENV{QT_TOOLCHAIN_ROOT}/bin/g++.exe"
    CACHE FILEPATH "" FORCE)

#
# C++ Standard
#
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#
# Native ビルド用の CMSIS-DSP ビルドオプションを設定
# 参考: https://github.com/ARM-software/CMSIS-DSP#how-to-install-as-a-cmake-package
#
set(HOST ON CACHE BOOL "")
set(DISABLEFLOAT16 ON CACHE BOOL "")
