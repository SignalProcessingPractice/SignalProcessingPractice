#
# カレントディレクトリ配下のファイルをすべて取得 (再帰探索はしない)
#
file(GLOB SRC_FILES 
    ${CMAKE_CURRENT_LIST_DIR}/src/common/frame_sync/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/common/frame_sync/*.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/common/pipeline/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/common/pipeline/*.hpp)

#
# Add a library target
#
add_library(SIGNAL_PROCESSING_PRACTICE_LIB
            STATIC 
            ${SRC_FILES})

#
# Set the PUBLIC include path
#
target_include_directories(
    SIGNAL_PROCESSING_PRACTICE_LIB
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/inc/)

#
# ETL の取り込み
#
include(FetchContent)

FetchContent_Declare(
    etl
    GIT_REPOSITORY https://github.com/ETLCPP/etl.git
    GIT_TAG        20.47.1
)

FetchContent_MakeAvailable(etl)

target_link_libraries(SIGNAL_PROCESSING_PRACTICE_LIB PRIVATE etl::etl)
