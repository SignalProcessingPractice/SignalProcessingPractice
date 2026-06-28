///
/// @file PipelineContext.hpp
///
#include "FrameSyncProcess.hpp"

class PipelineContext {
public:
    ///
    /// @name ctor, dtor.
    /// {@
    PipelineContext() = default;
    explicit PipelineContext(const FrameSyncProcessConfig& config);

    PipelineContext(const PipelineContext&) = default;
    auto operator=(const PipelineContext&) -> PipelineContext& = default;
    PipelineContext(PipelineContext&&) = default;
    auto operator=(PipelineContext&&) -> PipelineContext& = default;
    ~PipelineContext() = default;
    /// @}

    ///
    /// @name 公開関数.
    /// {@

    ///
    /// パイプライン起動.
    ///
    auto exec() -> void;

    ///
    /// 全 Strategy の内部状態をリセット.
    ///
    auto reset() -> void;

    ///
    /// 各 Strategy 設定.
    ///
    auto SetAquireStrategy(FrameSyncProcess::AudioAquireStrategy strategy) -> void;
    auto SetPreProcessStrategy(FrameSyncProcess::PreProcessStrategy strategy) -> void;
    auto SetOverlapStrategy(FrameSyncProcess::OverlapStrategy strategy) -> void;
    auto SetWindowStrategy(FrameSyncProcess::WindowStrategy strategy) -> void;
    auto SetFftStrategy(FrameSyncProcess::FftStrategy strategy) -> void;
    auto SetInferStrategy(FrameSyncProcess::InferStrategy strategy) -> void;
    auto SetPostProcessStrategy(FrameSyncProcess::PostProcessStrategy strategy) -> void;
    auto SetOverlapAddStrategy(FrameSyncProcess::OverlapAddStrategy strategy) -> void;
    auto SetOutputStrategy(FrameSyncProcess::AudioOutputStrategy strategy) -> void;
    /// @}

private:
    FrameSyncProcess::AudioAquireStrategy audio_aquire_strategy_;
    FrameSyncProcess::PreProcessStrategy pre_process_strategy_;
    FrameSyncProcess::OverlapStrategy overlap_strategy_;
    FrameSyncProcess::WindowStrategy window_strategy_;
    FrameSyncProcess::FftStrategy fft_strategy_;
    FrameSyncProcess::InferStrategy infer_strategy_;
    FrameSyncProcess::PostProcessStrategy post_process_strategy_;
    FrameSyncProcess::OverlapAddStrategy overlap_add_strategy_;
    FrameSyncProcess::AudioOutputStrategy audio_output_strategy_;
};
