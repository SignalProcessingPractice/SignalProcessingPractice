# CMake toolchain file

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(CMAKE_C_COMPILER clang CACHE STRING "C compiler" FORCE)
set(CMAKE_CXX_COMPILER clang++ CACHE STRING "C++ compiler" FORCE)

# コンパイラオプションを設定
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -Wall -Wextra -Werror")
# Win環境ではバイナリ埋め込みの文字列リテラルが標準で Shift JIS に変換されるため、ここでは UTF-8 を明示的に設定しておく
# XXX: 実行時の文字列リテラルはソースコードの文字コードが継承される、という話もあるので、この設定は不要かもしれない
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexec-charset=UTF-8")
