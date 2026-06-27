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
option(BUILD_APP "Build the application target" ON)
if(BUILD_APP)
    include(${CMAKE_SOURCE_DIR}/app/app.cmake)
endif()
include(${CMAKE_SOURCE_DIR}/lib/lib.cmake)
