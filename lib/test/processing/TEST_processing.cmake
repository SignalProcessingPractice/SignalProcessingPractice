#
# カレントディレクトリ配下のファイルをすべて取得 (再帰探索はしない)
#
file(GLOB SRC_FILES 
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/*.h
)

add_executable(
    TEST_PROCESSING_LIB
    ${SRC_FILES}
)

target_link_libraries(
    TEST_PROCESSING_LIB
    PUBLIC
    GTest::gtest_main
    Threads::Threads
    SIGNAL_PROCESSING_PRACTICE_LIB
)

add_test(
    NAME TEST_PROCESSING_LIB
    COMMAND TEST_PROCESSING_LIB
)

enable_testing()
