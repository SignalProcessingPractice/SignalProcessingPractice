///
/// @file PipelineContext.hpp
///
#include "FrameSyncProcess.hpp"
#include "Strategies/Overlapper.hpp"

class PipelineContext {
public:
    ///
    /// @name ctor, dtor.
    /// {@
    explicit PipelineContext();
    explicit PipelineContext(const FrameSyncProcessConfig& config);

    PipelineContext(const PipelineContext&) = default;
    PipelineContext& operator=(const PipelineContext&) = default;
    PipelineContext(PipelineContext&&) = default;
    PipelineContext& operator=(PipelineContext&&) = default;
    /// @}

    ///
    /// @name 公開関数.
    /// {@

    ///
    /// パイプライン起動.
    ///
    void exec(void) const;

    ///
    /// 各 Strategy 設定.
    ///
    /// @todo 設定処理の共通化を検討する. (Visitor パターンなど)
    ///
    void SetConfig(FrameSyncProcess::AquireTag tag, FrameSyncProcess::AudioAquireStrategy strategy);
    void SetConfig(FrameSyncProcess::PreProcessTag tag,
                   FrameSyncProcess::PreProcessStrategy strategy);
    void SetConfig(FrameSyncProcess::OverlapTag tag, FrameSyncProcess::OverlapStrategy strategy);
    void SetConfig(FrameSyncProcess::WindowTag tag, FrameSyncProcess::WindowStrategy strategy);
    void SetConfig(FrameSyncProcess::FftTag tag, FrameSyncProcess::FftStrategy strategy);
    void SetConfig(FrameSyncProcess::InferTag tag, FrameSyncProcess::InferStrategy strategy);
    void SetConfig(FrameSyncProcess::OutputTag tag, FrameSyncProcess::AudioOutputStrategy strategy);
    void SetConfig(FrameSyncProcess::PostProcessTag tag,
                   FrameSyncProcess::PostProcessStrategy strategy);
    void SetConfig(FrameSyncProcess::OverlapAddTag tag,
                   FrameSyncProcess::OverlapAddStrategy strategy);
    /// @}

private:
    ///
    /// オーディオフレーム獲得.
    ///
    FrameSyncProcess::AudioAquireStrategy audio_aquire_strategy_;
    ///
    /// 前処理.
    ///
    FrameSyncProcess::PreProcessStrategy pre_process_strategy_;
    ///
    /// オーバーラッピング.
    ///
    FrameSyncProcess::OverlapStrategy overlap_strategy_;
    ///
    /// 窓関数の積算.
    ///
    FrameSyncProcess::WindowStrategy window_strategy_;
    ///
    /// FFT.
    ///
    FrameSyncProcess::FftStrategy fft_strategy_;
    ///
    /// 推論.
    ///
    FrameSyncProcess::InferStrategy infer_strategy_;
    ///
    /// 後処理.
    ///
    FrameSyncProcess::PostProcessStrategy post_process_strategy_;
    ///
    /// Overlap-Add.
    ///
    FrameSyncProcess::OverlapAddStrategy overlap_add_strategy_;
    ///
    /// オーディオ出力.
    ///
    FrameSyncProcess::AudioOutputStrategy audio_output_strategy_;
};
