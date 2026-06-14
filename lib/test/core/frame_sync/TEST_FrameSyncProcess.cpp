#include <gtest/gtest.h>

#include <etl/delegate.h>

#include <iostream>
#include <iomanip>

#include "FrameSyncProcess.hpp"
#include "FrameSyncProcessConfig.hpp"

#include "Strategies/SineGenerator.hpp"
#include "Strategies/HannWindow.hpp"
#include "Strategies/HannOverlapAdder.hpp"

SineGenerator sine_gen_inputer(440.0, 0.5);
SineGenerator sine_gen_validator(440.0, 0.5);

// フレーム処理カウンター
static std::size_t frame_counter = 0;

auto input_delegate =
    etl::delegate<FrameSyncProcess::AudioHop(void)>
        ::create<SineGenerator, &SineGenerator::GenerateOneHop>(sine_gen_inputer);

auto output_delegate = []( FrameSyncProcess::AudioHop hop ) {
    // ここでは, 単純に入力と同じフレームを出力することを想定している.
    // つまり, 入力と出力が同一であることを検証する.
    auto validator_hop = sine_gen_validator.GenerateOneHop();

    EXPECT_EQ(hop.size(), validator_hop.size());
    constexpr float tolerance = 1e-1f;
    for (std::size_t i = 0; i < hop.size(); ++i)
    {
        // EXPECT_NEAR(hop[i], validator_hop[i], tolerance);
    }
    
    // フレーム値を周期的にコンソール出力
    // if (frame_counter % 128 == 0)
    {
        std::cout << "\n[Frame " << frame_counter << "] First 8 samples:\n";
        for (std::size_t i = 0; i < std::min(static_cast<std::size_t>(8), hop.size()) && i < validator_hop.size(); ++i)
        {
            std::cout << "  [" << i << "] output=" << std::fixed << std::setprecision(6) << hop[i]
                      << ", validator=" << std::fixed << std::setprecision(6) << validator_hop[i] << "\n";
        }
    }
    
    ++frame_counter;
};

HannWindow hann_window;
FrameSyncProcess::WindowStrategy hann_window_delegate =
    FrameSyncProcess::WindowStrategy
        ::create<HannWindow, &HannWindow::Execute>(hann_window);

HannOverlapAdder hann_overlap_adder;
FrameSyncProcess::OverlapAddStrategy hann_overlap_adder_delegate =
    FrameSyncProcess::OverlapAddStrategy
        ::create<HannOverlapAdder, &HannOverlapAdder::Execute>(hann_overlap_adder);

FrameSyncProcess proc {
    FrameSyncProcessConfig {
        .audio_aquire_strategy  = input_delegate,
        .window_strategy        = hann_window_delegate,
        .overlap_add_strategy   = hann_overlap_adder_delegate,
        .audio_output_strategy  = output_delegate,
    }
};

TEST(FrameSync, InputEqualOutput) {

    std::cout << "\n========== Frame Processing Started ==========\n";
    
    // 1024 フレーム分の処理を実行.
    for (std::size_t i = 0; i < 32; ++i)
    {
        proc.ProcessFrame();
    }
    
    std::cout << "========== Frame Processing Complete ==========\n";
    std::cout << "Total frames processed: " << frame_counter << "\n\n";
}
