///
/// @file TEST_FrameSyncProcess.cpp
///
#include <gtest/gtest.h>

#include <iomanip>
#include <iostream>

#include "FrameSyncProcess.hpp"
#include "FrameSyncProcessConfig.hpp"
#include "PipelineResult.hpp"
#include "Strategies/HannWindow.hpp"
#include "Strategies/Overlapper.hpp"
#include "Strategies/RectangleOverlapAdder.hpp"
#include "Strategies/SineGenerator.hpp"
#include "Strategies/null_strategies.hpp"

///
/// Hann 窓 + RectangleOverlapAdder 構成で, 2 フレーム処理した結果が
/// サイン波と一致することを確認する.
///
TEST(FrameSync, InputEqualOutput) {
    static constexpr std::size_t kDisplaySamples = 8U;
    static constexpr int kPrecision = 6;
    static constexpr float kTolerance = 1e-5F;

    SineGenerator sine_gen_inputer(SineGenerator::Params{SineGenerator::kDefaultFrequency,
                                                         SineGenerator::kDefaultAmplitude});
    SineGenerator sine_gen_validator(SineGenerator::Params{SineGenerator::kDefaultFrequency,
                                                           SineGenerator::kDefaultAmplitude});
    std::size_t frame_counter = 0U;

    class OutputValidator {
    public:
        OutputValidator(SineGenerator* validator, std::size_t* counter)
            : validator_(validator), counter_(counter) {}

        auto Exec(const FrameSyncProcess::AudioHop& hop) -> void {
            if (first_call_) {
                first_call_ = false;
                return;
            }

            auto validator_hop = validator_->Exec();
            EXPECT_EQ(hop.size(), validator_hop.size());
            for (std::size_t idx = 0; idx < hop.size(); ++idx) {
                EXPECT_NEAR(hop[idx], validator_hop[idx], kTolerance);
            }

            std::cout << "\n[Frame " << *counter_ << "] First " << kDisplaySamples
                      << " samples:\n";
            for (std::size_t idx = 0;
                 idx < std::min(kDisplaySamples, hop.size()) && idx < validator_hop.size();
                 ++idx) {
                std::cout << "  [" << idx << "] output=" << std::fixed
                          << std::setprecision(kPrecision) << hop[idx]
                          << ", validator=" << std::fixed << std::setprecision(kPrecision)
                          << validator_hop[idx] << "\n";
            }

            ++(*counter_);
        }

        auto Reset() -> void { first_call_ = true; }

    private:
        SineGenerator* validator_;
        std::size_t* counter_;
        bool first_call_{true};
    };

    HannWindow hann_window;
    RectangleOverlapAdder rectangle_overlap_adder;
    OutputValidator output_validator{&sine_gen_validator, &frame_counter};

    FrameSyncProcessConfig config;
    config.audio_aquire_strategy.bind(&sine_gen_inputer);
    config.window_strategy.bind(&hann_window);
    config.overlap_add_strategy.bind(&rectangle_overlap_adder);
    config.audio_output_strategy.bind(&output_validator);

    FrameSyncProcess proc{config};

    std::cout << "\n========== Frame Processing Started ==========\n";

    for (int idx = 0; idx < 2; ++idx) {
        proc.ProcessFrame();
    }
    std::cout << "\n";
    std::cout << "========== Frame Processing Complete ==========\n";
    std::cout << "Total frames processed: " << frame_counter << "\n\n";
}

///
/// デフォルト設定で生成した FrameSyncProcess が ProcessFrame() を実行できる.
///
TEST(FrameSync, DefaultConfigProcessFrameDoesNotCrash) {
    FrameSyncProcess proc{FrameSyncProcessConfig{}};
    proc.ProcessFrame();
}

///
/// ムーブコンストラクタ後の FrameSyncProcess が 2 フレーム処理した output_hop を正常に返す.
///
/// Overlapper の初期遅延があるため 2 フレーム以上処理してからサイン波の存在を確認する.
///
TEST(FrameSync, MoveCtorTransfersWorkingState) {
    constexpr int kFramesToProcess = 2;
    constexpr float kMinSignalMagnitude = 0.1F;
    SineGenerator gen{
        SineGenerator::Params{SineGenerator::kDefaultFrequency, SineGenerator::kDefaultAmplitude}};
    Overlapper overlapper;
    RectangleOverlapAdder adder;

    FrameSyncProcessConfig config;
    config.audio_aquire_strategy.bind(&gen);
    config.overlap_strategy.bind(&overlapper);
    config.overlap_add_strategy.bind(&adder);

    FrameSyncProcess src{config};
    FrameSyncProcess dst{std::move(src)};
    for (int idx = 0; idx < kFramesToProcess; ++idx) {
        dst.ProcessFrame();
    }

    PipelineResult result;
    dst.GetResult(&result);

    bool has_signal = false;
    for (const float sample : result.output_hop) {
        if (sample > kMinSignalMagnitude || sample < -kMinSignalMagnitude) {
            has_signal = true;
            break;
        }
    }
    EXPECT_TRUE(has_signal);
}

///
/// ムーブ代入後の FrameSyncProcess が 2 フレーム処理した output_hop を正常に返す.
///
/// Overlapper の初期遅延があるため 2 フレーム以上処理してからサイン波の存在を確認する.
///
TEST(FrameSync, MoveAssignmentTransfersWorkingState) {
    constexpr int kFramesToProcess = 2;
    constexpr float kMinSignalMagnitude = 0.1F;
    SineGenerator gen{
        SineGenerator::Params{SineGenerator::kDefaultFrequency, SineGenerator::kDefaultAmplitude}};
    Overlapper overlapper;
    RectangleOverlapAdder adder;

    FrameSyncProcessConfig config;
    config.audio_aquire_strategy.bind(&gen);
    config.overlap_strategy.bind(&overlapper);
    config.overlap_add_strategy.bind(&adder);

    FrameSyncProcess src{config};
    FrameSyncProcess dst;
    dst = std::move(src);
    for (int idx = 0; idx < kFramesToProcess; ++idx) {
        dst.ProcessFrame();
    }

    PipelineResult result;
    dst.GetResult(&result);

    bool has_signal = false;
    for (const float sample : result.output_hop) {
        if (sample > kMinSignalMagnitude || sample < -kMinSignalMagnitude) {
            has_signal = true;
            break;
        }
    }
    EXPECT_TRUE(has_signal);
}

///
/// GetResult() は 2 フレーム処理後の output_hop を正しく返す.
///
/// Overlapper の初期遅延があるため 2 フレーム以上処理してからサイン波の存在を確認する.
///
TEST(FrameSync, GetResultReturnsCurrentFrameData) {
    constexpr int kFramesToProcess = 2;
    constexpr float kMinSignalMagnitude = 0.1F;
    SineGenerator gen{
        SineGenerator::Params{SineGenerator::kDefaultFrequency, SineGenerator::kDefaultAmplitude}};
    Overlapper overlapper;
    RectangleOverlapAdder adder;

    FrameSyncProcessConfig config;
    config.audio_aquire_strategy.bind(&gen);
    config.overlap_strategy.bind(&overlapper);
    config.overlap_add_strategy.bind(&adder);

    FrameSyncProcess proc{config};
    for (int idx = 0; idx < kFramesToProcess; ++idx) {
        proc.ProcessFrame();
    }

    PipelineResult result;
    proc.GetResult(&result);

    bool has_signal = false;
    for (const float sample : result.output_hop) {
        if (sample > kMinSignalMagnitude || sample < -kMinSignalMagnitude) {
            has_signal = true;
            break;
        }
    }
    EXPECT_TRUE(has_signal);
}

///
/// SetConfig() で変更した Strategy は次の ProcessFrame() から有効になる.
///
/// SineGenerator で 2 フレーム warm-up して output_hop にサイン波が出ることを確認した後,
/// NullInput へ切り替えると次フレームの input_hop がゼロになることで検証する.
///
TEST(FrameSync, SetConfigAcquireStrategyAppliesAtFrameBoundary) {
    constexpr int kWarmUpFrames = 2;
    constexpr float kMinSignalMagnitude = 0.1F;
    SineGenerator gen{
        SineGenerator::Params{SineGenerator::kDefaultFrequency, SineGenerator::kDefaultAmplitude}};
    Overlapper overlapper;
    RectangleOverlapAdder adder;

    FrameSyncProcessConfig config;
    config.audio_aquire_strategy.bind(&gen);
    config.overlap_strategy.bind(&overlapper);
    config.overlap_add_strategy.bind(&adder);

    FrameSyncProcess proc{config};
    for (int idx = 0; idx < kWarmUpFrames; ++idx) {
        proc.ProcessFrame();
    }

    PipelineResult before;
    proc.GetResult(&before);
    bool had_signal = false;
    for (const float sample : before.output_hop) {
        if (sample > kMinSignalMagnitude || sample < -kMinSignalMagnitude) {
            had_signal = true;
            break;
        }
    }
    EXPECT_TRUE(had_signal);

    NullInput null_input;
    FrameSyncProcess::AudioAquireStrategy null_slot;
    null_slot.bind(&null_input);
    proc.SetConfig(FrameSyncProcess::AquireTag{}, null_slot);
    proc.ProcessFrame();

    PipelineResult after;
    proc.GetResult(&after);
    bool is_silent = true;
    for (const float sample : after.input_hop) {
        if (sample != 0.0F) {
            is_silent = false;
            break;
        }
    }
    EXPECT_TRUE(is_silent);
}

///
/// SetConfig() はパイプラインリセットを引き起こす.
///
/// SineGenerator で 2 フレーム warm-up して Overlapper に状態を持たせた後,
/// NullInput へ切り替えると Overlapper がリセットされ output_hop がゼロになることで検証する.
/// リセットなしでは Overlapper の残留状態により output_hop が非ゼロになる.
///
TEST(FrameSync, SetConfigTriggersPipelineReset) {
    constexpr int kWarmUpFrames = 2;
    constexpr float kMinSignalMagnitude = 0.1F;
    SineGenerator gen{
        SineGenerator::Params{SineGenerator::kDefaultFrequency, SineGenerator::kDefaultAmplitude}};
    Overlapper overlapper;
    RectangleOverlapAdder adder;

    FrameSyncProcessConfig config;
    config.audio_aquire_strategy.bind(&gen);
    config.overlap_strategy.bind(&overlapper);
    config.overlap_add_strategy.bind(&adder);

    FrameSyncProcess proc{config};
    for (int idx = 0; idx < kWarmUpFrames; ++idx) {
        proc.ProcessFrame();
    }

    PipelineResult before;
    proc.GetResult(&before);
    bool had_signal = false;
    for (const float sample : before.output_hop) {
        if (sample > kMinSignalMagnitude || sample < -kMinSignalMagnitude) {
            had_signal = true;
            break;
        }
    }
    EXPECT_TRUE(had_signal);

    NullInput null_input;
    FrameSyncProcess::AudioAquireStrategy null_slot;
    null_slot.bind(&null_input);
    proc.SetConfig(FrameSyncProcess::AquireTag{}, null_slot);
    proc.ProcessFrame();

    PipelineResult after;
    proc.GetResult(&after);
    for (const float sample : after.output_hop) {
        EXPECT_EQ(sample, 0.0F);
    }
}
