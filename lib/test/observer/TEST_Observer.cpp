#include <gtest/gtest.h>

#include "FrameSyncProcess.hpp"
#include "FrameSyncProcessConfig.hpp"
#include "PipelineResult.hpp"
#include "Strategies/FFT.hpp"
#include "Strategies/IFFT.hpp"
#include "Strategies/Overlapper.hpp"
#include "Strategies/RectangleOverlapAdder.hpp"
#include "Strategies/RectangleWindow.hpp"
#include "Strategies/SineGenerator.hpp"
#include "Strategies/null_strategies.hpp"

using ObserverDelegate = FrameSyncProcess::ObserverDelegate;

///
/// Observer の呼び出しカウントと最後に受け取った PipelineResult を記録するテスト用 Observer.
///
class ObserverSpy {
public:
    auto OnProcessFrame(const PipelineResult& result) -> void {
        ++call_count_;
        last_result_ = result;
    }

    [[nodiscard]] auto call_count() const -> int { return call_count_; }
    [[nodiscard]] auto last_result() const -> const PipelineResult& { return last_result_; }

    [[nodiscard]] auto as_delegate() -> ObserverDelegate {
        return ObserverDelegate::create<ObserverSpy, &ObserverSpy::OnProcessFrame>(*this);
    }

private:
    int call_count_{0};
    PipelineResult last_result_;
};

///
/// 出力ホップを外部バッファに取り込む Strategy.
/// PipelineResult::output_hop の検証用.
///
class OutputCapture {
public:
    explicit OutputCapture(FrameSyncProcess::AudioHop* dest) : dest_(dest) {}

    auto Exec(const FrameSyncProcess::AudioHop& hop) -> void { *dest_ = hop; }
    auto Reset() -> void { *dest_ = FrameSyncProcess::AudioHop{}; }

private:
    FrameSyncProcess::AudioHop* dest_;
};

///
/// ProcessFrame() の後に Observer が呼び出されることを確認する.
///
TEST(FrameSyncObserver, ObserverCalledAfterProcessFrame) {
    ObserverSpy spy;
    FrameSyncProcess proc{FrameSyncProcessConfig{}};
    proc.Attach(spy.as_delegate());

    EXPECT_EQ(spy.call_count(), 0);
    proc.ProcessFrame();
    EXPECT_EQ(spy.call_count(), 1);
    proc.ProcessFrame();
    EXPECT_EQ(spy.call_count(), 2);
}

///
/// Attach しない場合は Observer が呼び出されないことを確認する.
///
TEST(FrameSyncObserver, NotCalledWithoutAttach) {
    ObserverSpy spy;
    FrameSyncProcess proc{FrameSyncProcessConfig{}};

    proc.ProcessFrame();

    EXPECT_EQ(spy.call_count(), 0);
}

///
/// 複数の Observer がすべて通知されることを確認する.
///
TEST(FrameSyncObserver, MultipleObserversAllNotified) {
    ObserverSpy spy1;
    ObserverSpy spy2;
    ObserverSpy spy3;
    FrameSyncProcess proc{FrameSyncProcessConfig{}};
    proc.Attach(spy1.as_delegate());
    proc.Attach(spy2.as_delegate());
    proc.Attach(spy3.as_delegate());

    proc.ProcessFrame();

    EXPECT_EQ(spy1.call_count(), 1);
    EXPECT_EQ(spy2.call_count(), 1);
    EXPECT_EQ(spy3.call_count(), 1);
}

///
/// Detach した Observer はその後の ProcessFrame() で呼び出されないことを確認する.
///
TEST(FrameSyncObserver, DetachedObserverNotCalled) {
    ObserverSpy spy;
    FrameSyncProcess proc{FrameSyncProcessConfig{}};
    proc.Attach(spy.as_delegate());
    proc.ProcessFrame();
    EXPECT_EQ(spy.call_count(), 1);

    proc.Detach(spy.as_delegate());
    proc.ProcessFrame();

    EXPECT_EQ(spy.call_count(), 1);
}

///
/// Observer が受け取った PipelineResult と GetResult() が返す結果が一致することを確認する.
///
/// Overlapper の初期遅延を超えるため 2 フレーム処理してから比較する.
///
TEST(FrameSyncObserver, GetResultMatchesObserverResult) {
    constexpr SineGenerator::Params kParams{SineGenerator::kDefaultFrequency,
                                            SineGenerator::kDefaultAmplitude};
    constexpr int kFramesToProcess = 2;
    SineGenerator gen{kParams};
    Overlapper overlapper;
    RectangleWindow window;
    FFT fft;
    ThroughInfer infer;
    IFFT ifft;
    RectangleOverlapAdder adder;

    FrameSyncProcessConfig config;
    config.audio_aquire_strategy.bind(&gen);
    config.overlap_strategy.bind(&overlapper);
    config.window_strategy.bind(&window);
    config.fft_strategy.bind(&fft);
    config.infer_strategy.bind(&infer);
    config.post_process_strategy.bind(&ifft);
    config.overlap_add_strategy.bind(&adder);

    ObserverSpy spy;
    FrameSyncProcess proc{config};
    proc.Attach(spy.as_delegate());
    for (int idx = 0; idx < kFramesToProcess; ++idx) {
        proc.ProcessFrame();
    }

    PipelineResult get_result_out;
    proc.GetResult(&get_result_out);

    EXPECT_EQ(spy.last_result().input_hop, get_result_out.input_hop);
    EXPECT_EQ(spy.last_result().fft_frame, get_result_out.fft_frame);
    EXPECT_EQ(spy.last_result().output_hop, get_result_out.output_hop);
}

///
/// PipelineResult::output_hop が出力 Strategy に渡された hop と一致し,
/// かつ Sine 波信号が含まれることを確認する.
///
/// Overlapper の初期遅延を超えるため 2 フレーム処理する.
///
TEST(FrameSyncObserver, ObserverResultOutputHopMatchesPipelineOutput) {
    constexpr SineGenerator::Params kParams{SineGenerator::kDefaultFrequency,
                                            SineGenerator::kDefaultAmplitude};
    constexpr int kFramesToProcess = 2;
    constexpr float kMinSignalMagnitude = 0.1F;
    SineGenerator gen{kParams};
    Overlapper overlapper;
    RectangleWindow window;
    FFT fft;
    ThroughInfer infer;
    IFFT ifft;
    RectangleOverlapAdder adder;

    FrameSyncProcess::AudioHop captured_hop;
    OutputCapture capture{&captured_hop};

    FrameSyncProcessConfig config;
    config.audio_aquire_strategy.bind(&gen);
    config.overlap_strategy.bind(&overlapper);
    config.window_strategy.bind(&window);
    config.fft_strategy.bind(&fft);
    config.infer_strategy.bind(&infer);
    config.post_process_strategy.bind(&ifft);
    config.overlap_add_strategy.bind(&adder);
    config.audio_output_strategy.bind(&capture);

    ObserverSpy spy;
    FrameSyncProcess proc{config};
    proc.Attach(spy.as_delegate());
    for (int idx = 0; idx < kFramesToProcess; ++idx) {
        proc.ProcessFrame();
    }

    // Overlapper の遅延を超えた後, Sine 波信号が出力に現れることを確認する.
    bool has_signal = false;
    for (const float sample : spy.last_result().output_hop) {
        if (sample > kMinSignalMagnitude || sample < -kMinSignalMagnitude) {
            has_signal = true;
            break;
        }
    }
    EXPECT_TRUE(has_signal);

    // PipelineResult::output_hop が出力 Strategy に渡された hop と一致することを確認する.
    EXPECT_EQ(spy.last_result().output_hop, captured_hop);
}
