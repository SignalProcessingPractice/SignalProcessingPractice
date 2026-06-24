///
/// @file PipelineContext.cpp
///

#include "PipelineContext.hpp"

#include <cmath>

#include "FrameSyncProcess.hpp"
#include "FrameSyncProcessConfig.hpp"

PipelineContext::PipelineContext() : PipelineContext(FrameSyncProcessConfig{}) {
}

PipelineContext::PipelineContext(const FrameSyncProcessConfig& config)
    : audio_aquire_strategy_(config.audio_aquire_strategy),
      pre_process_strategy_(config.pre_process_strategy),
      overlap_strategy_(config.overlap_strategy),
      window_strategy_(config.window_strategy),
      fft_strategy_(config.fft_strategy),
      infer_strategy_(config.infer_strategy),
      post_process_strategy_(config.post_process_strategy),
      overlap_add_strategy_(config.overlap_add_strategy),
      audio_output_strategy_(config.audio_output_strategy) {
    /* do nothing. */
}

void PipelineContext::exec(void) const {
    ///
    /// Acquire Audio Data.
    ///
    auto in = this->audio_aquire_strategy_();

    ///
    /// Pre-Processing.
    ///
    auto pre_processed = this->pre_process_strategy_(std::move(in));

    ///
    /// オーバーラッピング.
    ///
    auto ov = this->overlap_strategy_(std::move(pre_processed));

    /// TODO: 推論実施前の, 窓関数積算 ~ FFT までの処理は, infered に何らかのフラグを設けることで実施有無を判別できるようにする.
    ///       推論モデルによっては, 周波数成分ではなく, 時間領域の特徴量を入力とするものもあるため, そのような処理可否判定を infered に持たせる.

    ///
    /// 窓関数の積算.
    ///
    auto win = this->window_strategy_(std::move(ov));

    ///
    /// FFT.
    ///
    auto ft = this->fft_strategy_(std::move(win));

    ///
    /// Infering.
    ///
    auto infered = this->infer_strategy_(std::move(ft));

    ///
    /// TODO: 推論実施後の, Post-Processing ~ Output までの処理は, infered に何らかのフラグを設けることで実施有無を判別できるようにする.
    ///       例えば, 声質変換であれば音声出力が必要となるが, キーワード識別であれば音声処理は不要となる. そのような音声出力可否判定を infered に持たせる.
    ///

    ///
    /// Post-Processing.
    ///
    auto post_processed = this->post_process_strategy_(std::move(infered));

    ///
    /// Overlap-Add.
    ///
    auto syn = this->overlap_add_strategy_(std::move(post_processed));

    ///
    /// Output.
    ///
    this->audio_output_strategy_(std::move(syn));
}

void PipelineContext::SetConfig(FrameSyncProcess::AquireTag tag,
                                FrameSyncProcess::AudioAquireStrategy strategy) {
    this->audio_aquire_strategy_ = strategy;
}

void PipelineContext::SetConfig(FrameSyncProcess::PreProcessTag tag,
                                FrameSyncProcess::PreProcessStrategy strategy) {
    this->pre_process_strategy_ = strategy;
}

void PipelineContext::SetConfig(FrameSyncProcess::InferTag tag,
                                FrameSyncProcess::InferStrategy strategy) {
    this->infer_strategy_ = strategy;
}

void PipelineContext::SetConfig(FrameSyncProcess::OutputTag tag,
                                FrameSyncProcess::AudioOutputStrategy strategy) {
    this->audio_output_strategy_ = strategy;
}
