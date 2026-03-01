#
# カレントディレクトリ配下のファイルをすべて取得 (再帰探索はしない)
#
file(GLOB SRC_FILES 
    ${CMAKE_CURRENT_LIST_DIR}/processing/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/processing/*.h)

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
