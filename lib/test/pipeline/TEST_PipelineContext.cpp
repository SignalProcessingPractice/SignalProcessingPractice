#include <gtest/gtest.h>

#include "FrameSyncProcessConfig.hpp"
#include "PipelineContext.hpp"
#include "Strategies/FFT.hpp"
#include "Strategies/IFFT.hpp"
#include "Strategies/Overlapper.hpp"
#include "Strategies/RectangleOverlapAdder.hpp"
#include "Strategies/RectangleWindow.hpp"
#include "Strategies/SineGenerator.hpp"
#include "Strategies/null_strategies.hpp"

class OutputCollector {
public:
    explicit OutputCollector(FrameSyncProcess::AudioHop* dest)
        : dest_(dest)
    {
    }

    auto Exec(const FrameSyncProcess::AudioHop& hop) -> void
    {
        *dest_ = hop;
    }
    auto Reset() -> void
    {
        *dest_ = FrameSyncProcess::AudioHop{};
    }

private:
    FrameSyncProcess::AudioHop* dest_;
};

TEST(PipelineContextReset, OutputMatchesFreshPipelineAfterReset)
{
    constexpr float kTolerance = 1e-5F;
    constexpr int kWarmupFrames = 3;
    constexpr int kCompareFrames = 2;
    constexpr SineGenerator::Params kGenParams{SineGenerator::kDefaultFrequency,
                                               SineGenerator::kDefaultAmplitude};

    FrameSyncProcess::AudioHop result_a;
    SineGenerator gen_a{kGenParams};
    Overlapper overlapper_a;
    RectangleWindow window_a;
    FFT fft_a;
    BypassInfer infer_a;
    IFFT ifft_a;
    RectangleOverlapAdder adder_a;
    OutputCollector collector_a{&result_a};

    FrameSyncProcessConfig config_a;
    config_a.audio_acquire_strategy.bind(&gen_a);
    config_a.overlap_strategy.bind(&overlapper_a);
    config_a.window_strategy.bind(&window_a);
    config_a.fft_strategy.bind(&fft_a);
    config_a.infer_strategy.bind(&infer_a);
    config_a.post_process_strategy.bind(&ifft_a);
    config_a.overlap_add_strategy.bind(&adder_a);
    config_a.audio_output_strategy.bind(&collector_a);

    PipelineContext pipeline_a{config_a};

    FrameSyncProcess::AudioHop result_b;
    SineGenerator gen_b{kGenParams};
    Overlapper overlapper_b;
    RectangleWindow window_b;
    FFT fft_b;
    BypassInfer infer_b;
    IFFT ifft_b;
    RectangleOverlapAdder adder_b;
    OutputCollector collector_b{&result_b};

    FrameSyncProcessConfig config_b;
    config_b.audio_acquire_strategy.bind(&gen_b);
    config_b.overlap_strategy.bind(&overlapper_b);
    config_b.window_strategy.bind(&window_b);
    config_b.fft_strategy.bind(&fft_b);
    config_b.infer_strategy.bind(&infer_b);
    config_b.post_process_strategy.bind(&ifft_b);
    config_b.overlap_add_strategy.bind(&adder_b);
    config_b.audio_output_strategy.bind(&collector_b);

    PipelineContext pipeline_b{config_b};

    for (int idx = 0; idx < kWarmupFrames; ++idx) {
        pipeline_a.exec();
    }
    pipeline_a.reset();

    for (int idx = 0; idx < kCompareFrames; ++idx) {
        pipeline_a.exec();
        pipeline_b.exec();
    }

    EXPECT_EQ(result_a.size(), result_b.size());
    for (std::size_t idx = 0; idx < result_a.size(); ++idx) {
        EXPECT_NEAR(result_a[idx], result_b[idx], kTolerance);
    }
}
