#include <etl/delegate.h>
#include <gtest/gtest.h>

#include <iomanip>
#include <iostream>

#include "FrameSyncProcess.hpp"
#include "FrameSyncProcessConfig.hpp"
#include "Strategies/HannWindow.hpp"
#include "Strategies/RectangleOverlapAdder.hpp"
#include "Strategies/SineGenerator.hpp"

TEST(FrameSync, InputEqualOutput) {
    static constexpr std::size_t kDisplaySamples = 8U;
    static constexpr int kPrecision = 6;
    static constexpr float kTolerance = 1e-5F;

    SineGenerator sine_gen_inputer(SineGenerator::Params{SineGenerator::kDefaultFrequency,
                                                         SineGenerator::kDefaultAmplitude});
    SineGenerator sine_gen_validator(SineGenerator::Params{SineGenerator::kDefaultFrequency,
                                                           SineGenerator::kDefaultAmplitude});
    std::size_t frame_counter = 0;

    auto input_delegate = etl::delegate<FrameSyncProcess::AudioHop()>::create<
            SineGenerator, &SineGenerator::GenerateOneHop>(sine_gen_inputer);

    HannWindow hann_window;
    auto hann_window_delegate =
            FrameSyncProcess::WindowStrategy::create<HannWindow, &HannWindow::Execute>(hann_window);

    RectangleOverlapAdder rectangle_overlap_adder;
    auto rectangle_overlap_adder_delegate = FrameSyncProcess::OverlapAddStrategy::create<
            RectangleOverlapAdder, &RectangleOverlapAdder::Execute>(rectangle_overlap_adder);

    auto output_lambda =
            [&sine_gen_validator, &frame_counter](const FrameSyncProcess::AudioHop &hop) {
        static bool first_time = true;

        if (first_time) {
            first_time = false;
            return;
        }

        auto validator_hop = sine_gen_validator.GenerateOneHop();
        EXPECT_EQ(hop.size(), validator_hop.size());
        for (std::size_t i = 0; i < hop.size(); ++i) {
            EXPECT_NEAR(hop[i], validator_hop[i], kTolerance);
        }

        std::cout << "\n[Frame " << frame_counter << "] First " << kDisplaySamples << " samples:\n";
        for (std::size_t i = 0;
             i < std::min(kDisplaySamples, hop.size()) && i < validator_hop.size(); ++i) {
            std::cout << "  [" << i << "] output=" << std::fixed << std::setprecision(kPrecision)
                      << hop[i] << ", validator=" << std::fixed << std::setprecision(kPrecision)
                      << validator_hop[i] << "\n";
        }

        ++frame_counter;
    };

    FrameSyncProcess proc{FrameSyncProcessConfig{
            .audio_aquire_strategy = input_delegate,
            .window_strategy = hann_window_delegate,
            .overlap_add_strategy = rectangle_overlap_adder_delegate,
            .audio_output_strategy = output_lambda,
    }};

    std::cout << "\n========== Frame Processing Started ==========\n";

    // 本パイプラインは 1 ホップ分の内部遅延を持つため、最初の出力はウォームアップとして扱う.
    for (int i = 0; i < 2; ++i) {
        proc.ProcessFrame();
    }
    std::cout << "\n";
    std::cout << "========== Frame Processing Complete ==========\n";
    std::cout << "Total frames processed: " << frame_counter << "\n\n";
}
