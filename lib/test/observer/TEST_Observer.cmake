file(GLOB SRC_FILES
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/*.h
)

add_executable(
    TEST_Observer
    ${SRC_FILES}
)

target_link_libraries(
    TEST_Observer
    PUBLIC
    GTest::gtest_main
    Threads::Threads
    SIGNAL_PROCESSING_PRACTICE_LIB
)

add_test(
    NAME TEST_Observer
    COMMAND TEST_Observer
)
