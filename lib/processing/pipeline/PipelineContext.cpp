///
/// @file PipelineContext.cpp
///

#include "PipelineContext.hpp"

#include "AudioFrame.hpp"

PipelineContext::PipelineContext ( 
    )
{
    /* do nothing. */
}

PipelineContext::AudioFrame
    PipelineContext::acquire (
        void
    ) const
{
    return this->audio_aquire_strategy_();
}

void 
    PipelineContext::exec (
        void
    ) const 
{
    ///
    /// Acquire Audio Data.
    ///
    /// TODO:   データ獲得に失敗した場合のエラー処理を実装する.
    ///         構造体をムーブセマンティクスで返却するのがモダンらしい.
    ///
    auto in = this->acquire();

    ///
    /// Pre-Processing.
    ///
    auto pp = this->preprocess(in); 

    ///
    /// Infering.
    ///
    auto infered = this->infer(pp);

    ///
    /// Out.
    ///
    this->out(infered);
}
