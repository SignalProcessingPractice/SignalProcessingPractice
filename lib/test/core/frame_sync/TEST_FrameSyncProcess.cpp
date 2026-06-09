#include <gtest/gtest.h>

#include <etl/delegate.h>

#include "FrameSyncProcess.hpp"
#include "FrameSyncProcessConfig.hpp"

#include "Strategies/SineGenerator.hpp"

SineGenerator sine_gen_inputer(440.0, 0.5);
SineGenerator sine_gen_validator(440.0, 0.5);

auto input_delegate =
    etl::delegate<FrameSyncProcess::AudioHop(void)>
        ::create<SineGenerator, &SineGenerator::GenerateOneHop>(sine_gen_inputer);

auto output_delegate = []( FrameSyncProcess::AudioHop hop ) {
    // ここでは, 単純に入力と同じフレームを出力することを想定している.
    // つまり, 入力と出力が同一であることを検証する.
    EXPECT_TRUE(hop == sine_gen_validator.GenerateOneHop());
};

FrameSyncProcess proc {
    FrameSyncProcessConfig {
        .audio_aquire_strategy = input_delegate,
        .audio_output_strategy = output_delegate,
    }
};

TEST(FrameSync, InputEqualOutput) {

    // 10 フレーム分の処理を実行.
    for (std::size_t i = 0; i < 10; ++i)
    {
        proc.ProcessFrame();
    }
}
