///
/// @file processing.hpp
///
#include <functional>

#include "AudioFrame.hpp"

class PipelineContext {

public:

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

    ///
    /// TODO: 要素数は後で再検討
    ///
    using AudioAquireStrategy = 
        std::function< AudioFrame( void ) >;


    ///
    /// コンストラクタ.
    ///
    explicit PipelineContext (
        );

    ///
    /// パイプライン起動.
    ///
    void 
        exec (
            void
        ) const;

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
            const AudioFrame& input_frame
        ) const;

    ///
    /// 推論.
    ///
    AudioFrame
        infer (
            const AudioFrame& preprocessed_frame
        ) const;

    ///
    /// オーディオ出力.
    ///
    void
        out (
            const AudioFrame& infered_frame
        ) const;

    AudioAquireStrategy audio_aquire_strategy_;

};
