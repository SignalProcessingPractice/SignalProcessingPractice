///
/// @file PipelineContext.cpp
///

#include "PipelineContext.hpp"
#include "FrameSyncProcess.hpp"

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

FrameSyncProcess::AudioFrame
    PipelineContext::acquire (
        void
    ) const
{
    return this->audio_aquire_strategy_();
}

FrameSyncProcess::AudioFrame
    PipelineContext::preprocess (
        FrameSyncProcess::AudioFrame &&frame
    ) const
{
    return this->pre_process_strategy_(std::move(frame));
}

FrameSyncProcess::AudioFrame
    PipelineContext::infer (
        FrameSyncProcess::AudioFrame &&frame
    ) const
{
    return this->infer_strategy_ (std::move(frame));
}

void
    PipelineContext::output (
        FrameSyncProcess::AudioFrame &&frame
    ) const
{
    this->audio_output_strategy_(std::move(frame));
}
