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
    auto pre_processed = this->preprocess(std::move(in)); 

    ///
    /// オーバーラッピング.
    ///
    auto ov = this->overlap(std::move(pre_processed));

    /// TODO: 推論実施前の, 窓関数積算 ~ FFT までの処理は, infered に何らかのフラグを設けることで実施有無を判別できるようにする.
    ///       推論モデルによっては, 周波数成分ではなく, 時間領域の特徴量を入力とするものもあるため, そのような処理可否判定を infered に持たせる.

    ///
    /// 窓関数の積算.
    ///
    auto win = this->window(std::move(ov));

    ///
    /// FFT.
    ///
    auto ft = this->fft(std::move(win));

    ///
    /// Infering.
    ///
    auto infered = this->infer(std::move(ft));

    ///
    /// TODO: 推論実施後の, Post-Processing ~ Output までの処理は, infered に何らかのフラグを設けることで実施有無を判別できるようにする.
    ///       例えば, 声質変換であれば音声出力が必要となるが, キーワード識別であれば音声処理は不要となる. そのような音声出力可否判定を infered に持たせる.
    ///

    ///
    /// Post-Processing.
    ///
    auto post_processed = this->post_processed(std::move(infered));

    ///
    /// Overlap-Add.
    ///
    auto syn = this->overlap_add(std::move(post_processed));

    ///
    /// Output.
    ///
    this->output(std::move(syn));
}

FrameSyncProcess::AudioHop
    PipelineContext::acquire (
        void
    ) const
{
    return this->audio_aquire_strategy_();
}

FrameSyncProcess::AudioHop
    PipelineContext::preprocess (
        FrameSyncProcess::AudioHop &&frame
    ) const
{
    return this->pre_process_strategy_(std::move(frame));
}

FrameSyncProcess::AudioFrame
    PipelineContext::overlap (
        FrameSyncProcess::AudioHop &&frame
    ) const
{
    return this->overlap_strategy_(std::move(frame));
}

FrameSyncProcess::AudioFrame
    PipelineContext::window (
        FrameSyncProcess::AudioFrame &&frame
    ) const
{
    return this->window_strategy_(std::move(frame));
}

FrameSyncProcess::AudioFrame
    PipelineContext::fft (
        FrameSyncProcess::AudioFrame &&frame
    ) const
{
    return this->fft_strategy_(std::move(frame));
}

FrameSyncProcess::AudioFrame
    PipelineContext::infer (
        FrameSyncProcess::AudioFrame &&frame
    ) const
{
    return this->infer_strategy_ (std::move(frame));
}

FrameSyncProcess::AudioFrame
    PipelineContext::post_processed (
        FrameSyncProcess::AudioFrame &&frame
    ) const
{
    return this->post_process_strategy_ (std::move(frame));
}

FrameSyncProcess::AudioHop
    PipelineContext::overlap_add (
        FrameSyncProcess::AudioFrame &&frame
    ) const
{
    return this->overlap_add_strategy_ (std::move(frame));
}

void
    PipelineContext::output (
        FrameSyncProcess::AudioHop &&frame
    ) const
{
    this->audio_output_strategy_(std::move(frame));
}

void 
    PipelineContext::SetConfig(
        FrameSyncProcess::AquireTag tag, 
        FrameSyncProcess::AudioAquireStrategy strategy
    )
{
    this->audio_aquire_strategy_ = strategy;
}

void 
    PipelineContext::SetConfig(
        FrameSyncProcess::PreProcessTag tag, 
        FrameSyncProcess::PreProcessStrategy strategy
    )
{
    this->pre_process_strategy_ = strategy;
}

void 
    PipelineContext::SetConfig(
        FrameSyncProcess::InferTag tag, 
        FrameSyncProcess::InferStrategy strategy
    )
{
    this->infer_strategy_ = strategy;
}

void 
    PipelineContext::SetConfig(
        FrameSyncProcess::OutputTag tag, 
        FrameSyncProcess::AudioOutputStrategy strategy
    )
{
    this->audio_output_strategy_ = strategy;
}
