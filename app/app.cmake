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
