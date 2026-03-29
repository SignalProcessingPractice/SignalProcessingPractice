#
# カレントディレクトリ配下のファイルをすべて取得 (再帰探索はしない)
#
file(GLOB SRC_FILES 
     ${CMAKE_CURRENT_LIST_DIR}/*.cpp
     ${CMAKE_CURRENT_LIST_DIR}/*.h
     ${CMAKE_CURRENT_LIST_DIR}/*.hpp
     )

add_executable(APP_WINDOWS
               ${SRC_FILES})

target_include_directories(APP_WINDOWS
                         PUBLIC
                         ${CMAKE_CURRENT_SOURCE_DIR}/lib/inc/)


#
# TODO: RtAudio を FetchContent で取り込む。プラットフォーム依存のスクリプトなので後で見直し
#
include(FetchContent)

FetchContent_Declare(
  rtaudio
  GIT_REPOSITORY https://github.com/thestk/rtaudio.git
  GIT_TAG        6.0.1  # 必要に応じて固定（例: 6.0.1 など）
)

# RtAudioのオプション設定（Fetch前に指定）
set(RTAUDIO_API_WASAPI ON CACHE BOOL "" FORCE)
set(RTAUDIO_API_DS OFF CACHE BOOL "" FORCE)
set(RTAUDIO_USE_PTHREADS OFF CACHE BOOL "" FORCE)
set(RTAUDIO_BUILD_C OFF CACHE BOOL "" FORCE)
set(RTAUDIO_BUILD_CPP ON CACHE BOOL "" FORCE)
set(RTAUDIO_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(RTAUDIO_BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(rtaudio)

find_package(Threads REQUIRED)

target_link_libraries(APP_WINDOWS
     PRIVATE
     rtaudio
     Threads::Threads
)
