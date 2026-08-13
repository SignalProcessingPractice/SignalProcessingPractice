#include <gtest/gtest.h>

#include "FrameSyncProcess.hpp"
#include "Strategies/Overlapper.hpp"
#include "Strategies/RectangleOverlapAdder.hpp"
#include "Strategies/SineGenerator.hpp"

TEST(SineGeneratorReset, OutputMatchesFreshGeneratorAfterReset)
{
    constexpr SineGenerator::Params kParams{SineGenerator::kDefaultFrequency,
                                            SineGenerator::kDefaultAmplitude};
    SineGenerator gen_a{kParams};
    SineGenerator gen_b{kParams};

    for (int idx = 0; idx < 3; ++idx) {
        gen_a.Exec();
    }
    gen_a.Reset();

    EXPECT_EQ(gen_a.Exec(), gen_b.Exec());
}

TEST(OverlapperReset, OutputMatchesFreshInstanceAfterReset)
{
    constexpr float kSampleScale = 0.001F;
    Overlapper overlapper_a;

    FrameSyncProcess::AudioHop test_hop;
    for (std::size_t idx = 0; idx < test_hop.size(); ++idx) {
        test_hop[idx] = static_cast<float>(idx) * kSampleScale;
    }

    overlapper_a.Exec(test_hop);
    overlapper_a.Exec(test_hop);
    overlapper_a.Reset();

    Overlapper overlapper_b;

    EXPECT_EQ(overlapper_a.Exec(test_hop), overlapper_b.Exec(test_hop));
}

TEST(RectangleOverlapAdderReset, OutputMatchesFreshInstanceAfterReset)
{
    constexpr float kSampleScale = 0.001F;
    RectangleOverlapAdder adder_a;

    FrameSyncProcess::AudioFrame test_frame;
    for (std::size_t idx = 0; idx < test_frame.size(); ++idx) {
        test_frame[idx] = static_cast<float>(idx) * kSampleScale;
    }

    adder_a.Exec(test_frame);
    adder_a.Reset();

    RectangleOverlapAdder adder_b;

    EXPECT_EQ(adder_a.Exec(test_frame), adder_b.Exec(test_frame));
}
