///
/// @file processing.hpp
///
#include <etl/delegate.h>

#include "AudioFrame.hpp"

class PipelineContext {

public:

///
/// @name オーディオフレーム.
/// {@

    ///
    /// オーディオフレームの 1 フレーム辺りのサンプル数.
    ///
    /// TODO: サンプル数は後で再検討する.
    ///
    static constexpr std::size_t audio_frame_length = 256;

    ///
    /// オーディオフレーム.
    ///
    using AudioFrame =
        AudioFrameTemplate<audio_frame_length, double>;

/// @}

///
/// @name Strategy.
/// {@

    static constexpr std::size_t strategy_size = 64;

    ///
    /// オーディオフレーム獲得.
    ///
    using AudioAquireStrategy = 
        etl::delegate< AudioFrame( void ) >;

    ///
    /// オーディオ前処理.
    ///
    using PreProcessStrategy = 
        etl::delegate< AudioFrame( AudioFrame &&frame ) >;

    ///
    /// 推論.
    ///
    using InferStrategy = 
        etl::delegate< AudioFrame( AudioFrame &&frame ) >;

    ///
    /// オーディオ出力.
    ///
    using AudioOutputStrategy = 
        etl::delegate< void( AudioFrame &&frame ) >;

/// @}

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
