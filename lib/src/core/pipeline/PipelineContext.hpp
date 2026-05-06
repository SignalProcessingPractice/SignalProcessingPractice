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
/// @}

private:

    ///
    /// オーディオフレーム獲得.
    ///
    AudioFrame
        acquire (
            void
        ) const;

    ///
    /// 前処理.
    ///
    AudioFrame
        preprocess (
            AudioFrame&& frame
        ) const;

    ///
    /// 推論.
    ///
    AudioFrame
        infer (
            AudioFrame&& frame
        ) const;

    ///
    /// オーディオ出力.
    ///
    void
        output (
            AudioFrame&& frame
        ) const;


    AudioAquireStrategy     audio_aquire_strategy_;
    PreProcessStrategy      pre_process_strategy_;
    InferStrategy           infer_strategy_;
    AudioOutputStrategy     audio_output_startegy_;

};
