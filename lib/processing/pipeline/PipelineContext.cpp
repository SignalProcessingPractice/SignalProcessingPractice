///
/// @file PipelineContext.cpp
///

#include "PipelineContext.hpp"

PipelineContext::PipelineContext (
    )
{
    /* do nothing. */
}

void 
    PipelineContext::exec (
        void
    ) const 
{
    ///
    /// Acquire Audio Data.
    ///
    auto in = this->acquire();

    ///
    /// Pre-Processing.
    ///
    auto pp = this->preprocess(std::move(in)); 

    ///
    /// Infering.
    ///
    auto infered = this->infer(std::move(pp));

    ///
    /// Output.
    ///
    this->output(std::move(infered));
}

PipelineContext::AudioFrame
    PipelineContext::acquire (
        void
    ) const
{
    return this->audio_aquire_strategy_();
}

PipelineContext::AudioFrame
    PipelineContext::preprocess (
        AudioFrame &&frame
    ) const
{
    return this->pre_process_strategy_(std::move(frame));
}
