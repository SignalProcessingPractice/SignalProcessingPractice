///
/// @file processing.hpp
///
#include "FrameSyncProcess.hpp"

class PipelineContext {

public:

///
/// @name ctor, dtor.
/// {@
    explicit PipelineContext (
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
            FrameSyncProcess::InferTag tag, 
            FrameSyncProcess::InferStrategy strategy
        );
    void 
        SetConfig(
            FrameSyncProcess::OutputTag tag, 
            FrameSyncProcess::AudioOutputStrategy strategy
        );
/// @}

private:

    ///
    /// オーディオフレーム獲得.
    ///
    FrameSyncProcess::AudioFrame
        acquire (
            void
        ) const;

    ///
    /// 前処理.
    ///
    FrameSyncProcess::AudioFrame
        preprocess (
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
    /// オーディオ出力.
    ///
    void
        output (
            FrameSyncProcess::AudioFrame&& frame
        ) const;


    FrameSyncProcess::AudioAquireStrategy     audio_aquire_strategy_;
    FrameSyncProcess::PreProcessStrategy      pre_process_strategy_;
    FrameSyncProcess::InferStrategy           infer_strategy_;
    FrameSyncProcess::AudioOutputStrategy     audio_output_strategy_;

};
