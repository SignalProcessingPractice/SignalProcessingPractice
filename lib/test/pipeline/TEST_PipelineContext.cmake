file(GLOB SRC_FILES
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/*.h
)

add_executable(
    TEST_PipelineContext
    ${SRC_FILES}
)

target_link_libraries(
    TEST_PipelineContext
    PUBLIC
    GTest::gtest_main
    Threads::Threads
    SIGNAL_PROCESSING_PRACTICE_LIB
)

target_include_directories(
    TEST_PipelineContext
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../src/core/pipeline/)

add_test(
    NAME TEST_PipelineContext
    COMMAND TEST_PipelineContext
)
