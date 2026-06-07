#include <gtest/gtest.h>

#include <etl/delegate.h>

#include "FrameSyncProcess.hpp"

#include "Strategies/SineGenerator.hpp"

FrameSyncProcess proc;

SineGenerator sine_gen_inputer(440.0, 0.5);
SineGenerator sine_gen_validator(440.0, 0.5);

TEST(FrameSync, InputEqualOutput) {

    auto input_delegate =
    etl::delegate<FrameSyncProcess::AudioHop(void)>
        ::create<SineGenerator, &SineGenerator::GenerateOneHop>(sine_gen_inputer);

    proc.SetConfig(
        FrameSyncProcess::AquireTag{},
        input_delegate
    );

    auto output_delegate = []( FrameSyncProcess::AudioHop hop ) {
        // ここでは, 単純に入力と同じフレームを出力することを想定している.
        // つまり, 入力と出力が同一であることを検証する.
        EXPECT_TRUE(hop == sine_gen_validator.GenerateOneHop());
    };
    proc.SetConfig(
        FrameSyncProcess::OutputTag{},
        output_delegate
    );

    auto preprocess_delegate = []( FrameSyncProcess::AudioHop hop ) {
        // 何もしないでフレームをそのまま返す.
        return hop;
    };
    proc.SetConfig(
        FrameSyncProcess::PreProcessTag{},
        preprocess_delegate
    );

    auto infer_delegate = []( FrameSyncProcess::AudioFrame frame ) {
        // 何もしないでフレームをそのまま返す.
        return frame;
    };
    proc.SetConfig(
        FrameSyncProcess::InferTag{},
        infer_delegate
    );

    // 10 フレーム分の処理を実行.
    for (std::size_t i = 0; i < 10; ++i)
    {
        proc.ProcessFrame();
    }
}
