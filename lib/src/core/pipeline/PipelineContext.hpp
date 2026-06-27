///
/// @file PipelineContext.hpp
///
#include "FrameSyncProcess.hpp"
#include "Strategies/null_strategies.hpp"

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
    void exec() const;

    ///
    /// 各 Strategy 設定.
    ///
    void SetAquireStrategy(FrameSyncProcess::AudioAquireStrategy strategy);
    void SetPreProcessStrategy(FrameSyncProcess::PreProcessStrategy strategy);
    void SetOverlapStrategy(FrameSyncProcess::OverlapStrategy strategy);
    void SetWindowStrategy(FrameSyncProcess::WindowStrategy strategy);
    void SetFftStrategy(FrameSyncProcess::FftStrategy strategy);
    void SetInferStrategy(FrameSyncProcess::InferStrategy strategy);
    void SetPostProcessStrategy(FrameSyncProcess::PostProcessStrategy strategy);
    void SetOverlapAddStrategy(FrameSyncProcess::OverlapAddStrategy strategy);
    void SetOutputStrategy(FrameSyncProcess::AudioOutputStrategy strategy);
    /// @}

private:
    ///
    /// オーディオフレーム獲得.
    ///
    FrameSyncProcess::AudioAquireStrategy audio_aquire_strategy_{null_input};
    ///
    /// 前処理.
    ///
    FrameSyncProcess::PreProcessStrategy pre_process_strategy_{through_preprocess};
    ///
    /// オーバーラッピング.
    ///
    FrameSyncProcess::OverlapStrategy overlap_strategy_{null_overlap};
    ///
    /// 窓関数の積算.
    ///
    FrameSyncProcess::WindowStrategy window_strategy_{null_window};
    ///
    /// FFT.
    ///
    FrameSyncProcess::FftStrategy fft_strategy_{null_fft};
    ///
    /// 推論.
    ///
    FrameSyncProcess::InferStrategy infer_strategy_{through_infer};
    ///
    /// 後処理.
    ///
    FrameSyncProcess::PostProcessStrategy post_process_strategy_{through_postprocess};
    ///
    /// Overlap-Add.
    ///
    FrameSyncProcess::OverlapAddStrategy overlap_add_strategy_{null_overlap_add};
    ///
    /// オーディオ出力.
    ///
    FrameSyncProcess::AudioOutputStrategy audio_output_strategy_{null_output};
};
