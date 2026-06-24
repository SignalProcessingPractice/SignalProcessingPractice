#include <etl/delegate.h>
#include <gtest/gtest.h>

#include <iomanip>
#include <iostream>

#include "FrameSyncProcess.hpp"
#include "FrameSyncProcessConfig.hpp"
#include "Strategies/HannWindow.hpp"
#include "Strategies/RectangleOverlapAdder.hpp"
#include "Strategies/SineGenerator.hpp"

SineGenerator sine_gen_inputer(440.0, 0.5);
SineGenerator sine_gen_validator(440.0, 0.5);

// フレーム処理カウンタ
static std::size_t frame_counter = 0;

auto input_delegate = etl::delegate<FrameSyncProcess::AudioHop(
        void)>::create<SineGenerator, &SineGenerator::GenerateOneHop>(sine_gen_inputer);

HannWindow hann_window;
FrameSyncProcess::WindowStrategy hann_window_delegate =
        FrameSyncProcess::WindowStrategy ::create<HannWindow, &HannWindow::Execute>(hann_window);

RectangleOverlapAdder rectangle_overlap_adder;
FrameSyncProcess::OverlapAddStrategy rectangle_overlap_adder_delegate =
        FrameSyncProcess::OverlapAddStrategy ::create<RectangleOverlapAdder,
                                                      &RectangleOverlapAdder::Execute>(
                rectangle_overlap_adder);

auto output_delegate = [](FrameSyncProcess::AudioHop hop) {
    static bool first_time = true;

    if (first_time) {
        first_time = false;
        return;
    }

    auto validator_hop = sine_gen_validator.GenerateOneHop();
    EXPECT_EQ(hop.size(), validator_hop.size());
    constexpr float tolerance = 1e-5f;
    for (std::size_t i = 0; i < hop.size(); ++i) {
        EXPECT_NEAR(hop[i], validator_hop[i], tolerance);
    }

    // フレーム値をコンソール出力
    std::cout << "\n[Frame " << frame_counter << "] First 8 samples:\n";
    for (std::size_t i = 0;
         i < std::min(static_cast<std::size_t>(8), hop.size()) && i < validator_hop.size(); ++i) {
        std::cout << "  [" << i << "] output=" << std::fixed << std::setprecision(6) << hop[i]
                  << ", validator=" << std::fixed << std::setprecision(6) << validator_hop[i]
                  << "\n";
    }

    ++frame_counter;
};

FrameSyncProcess proc{FrameSyncProcessConfig{
        .audio_aquire_strategy = input_delegate,
        .window_strategy = hann_window_delegate,
        .overlap_add_strategy = rectangle_overlap_adder_delegate,
        .audio_output_strategy = output_delegate,
}};

TEST(FrameSync, InputEqualOutput) {
    std::cout << "\n========== Frame Processing Started ==========\n";

    // 本パイプラインは 1 ホップ分の内部遅延を持つため、最初の出力はウォームアップとして扱う.
    for (int i = 0; i < 2; ++i) {
        proc.ProcessFrame();
    }
    std::cout << "\n";
    std::cout << "========== Frame Processing Complete ==========\n";
    std::cout << "Total frames processed: " << frame_counter << "\n\n";
}
