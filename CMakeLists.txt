cmake_minimum_required(VERSION 3.28)
project(CPP_PRACTICE CXX)

#
# Set the C++ standard
#
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#
# Enable generation of compile_commands.json
#
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#
# Add subdirectories
#
include(${CMAKE_SOURCE_DIR}/app/app.cmake)
include(${CMAKE_SOURCE_DIR}/lib/lib.cmake)
