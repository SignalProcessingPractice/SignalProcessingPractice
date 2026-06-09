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
    explicit PipelineContext (
        );
    explicit PipelineContext (
        const FrameSyncProcessConfig& config
        );

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
    void 
        exec (
            void
        ) const;

    ///
    /// 各 Strategy 設定.
    ///
    /// @todo 設定処理の共通化を検討する. (Visitor パターンなど)
    ///
    void 
        SetConfig(
            FrameSyncProcess::AquireTag tag, 
            FrameSyncProcess::AudioAquireStrategy strategy
        );
    void 
        SetConfig(
            FrameSyncProcess::PreProcessTag tag, 
            FrameSyncProcess::PreProcessStrategy strategy
        );
    void 
        SetConfig(
            FrameSyncProcess::OverlapTag tag, 
            FrameSyncProcess::OverlapStrategy strategy
        );
    void 
        SetConfig(
            FrameSyncProcess::WindowTag tag, 
            FrameSyncProcess::WindowStrategy strategy
        );
    void 
        SetConfig(
            FrameSyncProcess::FftTag tag, 
            FrameSyncProcess::FftStrategy strategy
        );
    void 
        SetConfig(
            FrameSyncProcess::InferTag tag, 
            FrameSyncProcess::InferStrategy strategy
        );
    void 
        SetConfig(
            FrameSyncProcess::OutputTag tag, 
            FrameSyncProcess::AudioOutputStrategy strategy
        );
    void 
        SetConfig(
            FrameSyncProcess::PostProcessTag tag, 
            FrameSyncProcess::PostProcessStrategy strategy
        );
    void 
        SetConfig(
            FrameSyncProcess::OverlapAddTag tag, 
            FrameSyncProcess::OverlapAddStrategy strategy
        );
/// @}

private:

    ///
    /// オーディオフレーム獲得.
    ///
    FrameSyncProcess::AudioHop
        acquire (
            void
        ) const;

    ///
    /// 前処理.
    ///
    FrameSyncProcess::AudioHop
        preprocess (
            FrameSyncProcess::AudioHop&& frame
        ) const;

    ///
    /// オーバーラッピング.
    ///
    FrameSyncProcess::AudioFrame
        overlap (
            FrameSyncProcess::AudioHop&& frame
        ) const;

    ///
    /// 窓関数の積算.
    ///
    FrameSyncProcess::AudioFrame
        window (
            FrameSyncProcess::AudioFrame&& frame
        ) const;

    ///
    /// FFT.
    ///
    FrameSyncProcess::AudioFrame
        fft (
            FrameSyncProcess::AudioFrame&& frame
        ) const;

    ///
    /// 推論.
    ///
    FrameSyncProcess::AudioFrame
        infer (
            FrameSyncProcess::AudioFrame&& frame
        ) const;

    ///
    /// 後処理.
    ///
    FrameSyncProcess::AudioFrame
        post_processed (
            FrameSyncProcess::AudioFrame&& frame
        ) const;

    ///
    /// Overlap-Add.
    ///
    FrameSyncProcess::AudioHop
        overlap_add (
            FrameSyncProcess::AudioFrame&& frame
        ) const;

    ///
    /// オーディオ出力.
    ///
    void
        output (
            FrameSyncProcess::AudioHop&& frame
        ) const;


    FrameSyncProcess::AudioAquireStrategy     audio_aquire_strategy_;
    FrameSyncProcess::PreProcessStrategy      pre_process_strategy_;
    FrameSyncProcess::OverlapStrategy         overlap_strategy_;
    FrameSyncProcess::WindowStrategy          window_strategy_;
    FrameSyncProcess::FftStrategy             fft_strategy_;
    FrameSyncProcess::InferStrategy           infer_strategy_;
    FrameSyncProcess::PostProcessStrategy     post_process_strategy_;
    FrameSyncProcess::OverlapAddStrategy      overlap_add_strategy_;
    FrameSyncProcess::AudioOutputStrategy     audio_output_strategy_;

    ///
    /// デフォルト Strategy.
    ///
    Overlapper default_overlapper_;

};
