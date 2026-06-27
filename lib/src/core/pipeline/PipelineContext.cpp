///
/// @file PipelineContext.cpp
///

#include "PipelineContext.hpp"

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

void PipelineContext::exec() const {
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

void PipelineContext::SetAquireStrategy(FrameSyncProcess::AudioAquireStrategy strategy) {
    audio_aquire_strategy_ = strategy;
}

void PipelineContext::SetPreProcessStrategy(FrameSyncProcess::PreProcessStrategy strategy) {
    pre_process_strategy_ = strategy;
}

void PipelineContext::SetOverlapStrategy(FrameSyncProcess::OverlapStrategy strategy) {
    overlap_strategy_ = strategy;
}

void PipelineContext::SetWindowStrategy(FrameSyncProcess::WindowStrategy strategy) {
    window_strategy_ = strategy;
}

void PipelineContext::SetFftStrategy(FrameSyncProcess::FftStrategy strategy) {
    fft_strategy_ = strategy;
}

void PipelineContext::SetInferStrategy(FrameSyncProcess::InferStrategy strategy) {
    infer_strategy_ = strategy;
}

void PipelineContext::SetPostProcessStrategy(FrameSyncProcess::PostProcessStrategy strategy) {
    post_process_strategy_ = strategy;
}

void PipelineContext::SetOverlapAddStrategy(FrameSyncProcess::OverlapAddStrategy strategy) {
    overlap_add_strategy_ = strategy;
}

void PipelineContext::SetOutputStrategy(FrameSyncProcess::AudioOutputStrategy strategy) {
    audio_output_strategy_ = strategy;
}
