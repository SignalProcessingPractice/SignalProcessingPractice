#include <gtest/gtest.h>

#include "processing_dummy.hpp"

//
// Demonstrate some basic assertions.
//
TEST(Processing, DummyProcessingTest) {
    DummyProcessing dp;

    EXPECT_EQ(dp.dummy_calc(1, 2), 3);
    EXPECT_EQ(dp.dummy_calc(2, 3), 5);
}
