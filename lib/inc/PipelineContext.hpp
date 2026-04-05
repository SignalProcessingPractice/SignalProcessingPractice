///
/// @file processing.hpp
///

#include <functional>
#include <array>

#include "AudioFrame.hpp"

class PipelineContext {

#if 0
public:
    bool attach( SignalProcessingObserver* observer );
    bool detach( SignalProcessingObserver* observer );

    void notify( StateChange property );

private:


    std::set<PersonObserver*> observers_;
#endif

private:


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
    using AudioInputStrategy = 
        std::function< AudioFrame( void ) >;


    ///
    /// コンストラクタ.
    ///
    explicit PipelineContext (
            AudioInputStrategy audio_input_strategy
        );

    ///
    /// パイプライン起動.
    ///
    void 
        exec (
            void
        ) const;

private:

    AudioFrame
        acquire (
            void
        ) const;

    AudioFrame
        preprocess (
            const AudioFrame& input_frame
        ) const;

    AudioFrame
        infer (
            const AudioFrame& preprocessed_frame
        ) const;

    ///
    /// TODO: 出力処理は Observer パターンで実装したい.
    ///

    AudioInputStrategy audio_input_strategy_;

};
