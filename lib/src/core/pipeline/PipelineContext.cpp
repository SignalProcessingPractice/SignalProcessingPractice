///
/// @file PipelineContext.cpp
///

#include "PipelineContext.hpp"

#include <utility>

#include "FrameSyncProcess.hpp"
#include "FrameSyncProcessConfig.hpp"

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

auto PipelineContext::exec() -> void {
    auto input_hop = this->audio_aquire_strategy_();
    auto pre_processed = this->pre_process_strategy_(input_hop);
    auto overlapped = this->overlap_strategy_(pre_processed);
    auto win_applied = this->window_strategy_(overlapped);
    auto fft_result = this->fft_strategy_(win_applied);
    auto infered = this->infer_strategy_(fft_result);
    auto post_processed = this->post_process_strategy_(infered);
    auto synthesized = this->overlap_add_strategy_(post_processed);
    this->audio_output_strategy_(synthesized);
}

auto PipelineContext::reset() -> void {
    audio_aquire_strategy_.reset();
    pre_process_strategy_.reset();
    overlap_strategy_.reset();
    window_strategy_.reset();
    fft_strategy_.reset();
    infer_strategy_.reset();
    post_process_strategy_.reset();
    overlap_add_strategy_.reset();
    audio_output_strategy_.reset();
}

auto PipelineContext::SetAquireStrategy(FrameSyncProcess::AudioAquireStrategy strategy) -> void {
    audio_aquire_strategy_ = std::move(strategy);
}

auto PipelineContext::SetPreProcessStrategy(FrameSyncProcess::PreProcessStrategy strategy) -> void {
    pre_process_strategy_ = std::move(strategy);
}

auto PipelineContext::SetOverlapStrategy(FrameSyncProcess::OverlapStrategy strategy) -> void {
    overlap_strategy_ = std::move(strategy);
}

auto PipelineContext::SetWindowStrategy(FrameSyncProcess::WindowStrategy strategy) -> void {
    window_strategy_ = std::move(strategy);
}

auto PipelineContext::SetFftStrategy(FrameSyncProcess::FftStrategy strategy) -> void {
    fft_strategy_ = std::move(strategy);
}

auto PipelineContext::SetInferStrategy(FrameSyncProcess::InferStrategy strategy) -> void {
    infer_strategy_ = std::move(strategy);
}

auto PipelineContext::SetPostProcessStrategy(FrameSyncProcess::PostProcessStrategy strategy)
        -> void {
    post_process_strategy_ = std::move(strategy);
}

auto PipelineContext::SetOverlapAddStrategy(FrameSyncProcess::OverlapAddStrategy strategy) -> void {
    overlap_add_strategy_ = std::move(strategy);
}

auto PipelineContext::SetOutputStrategy(FrameSyncProcess::AudioOutputStrategy strategy) -> void {
    audio_output_strategy_ = std::move(strategy);
}
