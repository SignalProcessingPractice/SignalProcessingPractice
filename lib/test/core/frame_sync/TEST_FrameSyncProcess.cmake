#
# カレントディレクトリ配下のファイルをすべて取得 (再帰探索はしない)
#
file(GLOB SRC_FILES 
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/*.h
)

add_executable(
    TEST_FrameSyncProcess
    ${SRC_FILES}
)

target_link_libraries(
    TEST_FrameSyncProcess
    PUBLIC
    GTest::gtest_main
    Threads::Threads
    SIGNAL_PROCESSING_PRACTICE_LIB
)

add_test(
    NAME TEST_FrameSyncProcess
    COMMAND TEST_FrameSyncProcess
)

enable_testing()
