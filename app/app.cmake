#
# カレントディレクトリ配下のファイルを取得
#
file(GLOB SRC_FILES
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/*.h
    ${CMAKE_CURRENT_LIST_DIR}/*.hpp
)

add_executable(APP
    ${SRC_FILES}
)

target_include_directories(APP
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/inc
)

#
# RtAudio
#
include(FetchContent)

#
# RtAudio オプション初期化
#
set(RTAUDIO_API_WASAPI OFF CACHE BOOL "" FORCE)
set(RTAUDIO_API_DS OFF CACHE BOOL "" FORCE)
set(RTAUDIO_API_ALSA OFF CACHE BOOL "" FORCE)
set(RTAUDIO_API_PULSE OFF CACHE BOOL "" FORCE)
set(RTAUDIO_API_JACK OFF CACHE BOOL "" FORCE)

if(WIN32)

    message(STATUS "RtAudio backend: WASAPI")

    set(RTAUDIO_API_WASAPI ON CACHE BOOL "" FORCE)
    set(RTAUDIO_USE_PTHREADS OFF CACHE BOOL "" FORCE)

elseif(UNIX)

    message(STATUS "RtAudio backend: ALSA")

    set(RTAUDIO_API_ALSA ON CACHE BOOL "" FORCE)
    set(RTAUDIO_USE_PTHREADS ON CACHE BOOL "" FORCE)

endif()

set(RTAUDIO_BUILD_C OFF CACHE BOOL "" FORCE)
set(RTAUDIO_BUILD_CPP ON CACHE BOOL "" FORCE)
set(RTAUDIO_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(RTAUDIO_BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    rtaudio
    GIT_REPOSITORY https://github.com/thestk/rtaudio.git
    GIT_TAG        6.0.1
)

FetchContent_MakeAvailable(rtaudio)

find_package(Threads REQUIRED)

target_link_libraries(APP
    PRIVATE
        rtaudio
        Threads::Threads
)