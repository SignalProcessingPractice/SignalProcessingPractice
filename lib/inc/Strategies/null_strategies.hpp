///
/// @file null_strategy.hpp
///
#pragma once

#include "FrameSyncProcess.hpp"

///
/// @brief Null input strategy.
///
auto null_input() -> FrameSyncProcess::AudioHop;

///
/// @brief Null preprocessing strategy.
///
auto through_preprocess(const FrameSyncProcess::AudioHop &frame) -> FrameSyncProcess::AudioHop;

///
/// @brief Null postprocessing strategy.
///
auto through_postprocess(const FrameSyncProcess::AudioFrame &frame) -> FrameSyncProcess::AudioFrame;

///
/// @brief Null overlap strategy.
///
auto null_overlap(const FrameSyncProcess::AudioHop &frame) -> FrameSyncProcess::AudioFrame;

///
/// @brief Null Window strategy.
///
auto null_window(const FrameSyncProcess::AudioFrame &frame) -> FrameSyncProcess::AudioFrame;

///
/// @brief Null FFT strategy.
///
auto null_fft(const FrameSyncProcess::AudioFrame &frame) -> FrameSyncProcess::AudioFrame;

///
/// @brief Through Infer strategy.
///
auto through_infer(const FrameSyncProcess::AudioFrame &frame) -> FrameSyncProcess::AudioFrame;

///
/// @brief Null Infer strategy.
///
auto null_infer(const FrameSyncProcess::AudioFrame &frame) -> FrameSyncProcess::AudioFrame;

///
/// @brief Null overlap-add strategy.
///
auto null_overlap_add(const FrameSyncProcess::AudioFrame &frame) -> FrameSyncProcess::AudioHop;

///
/// @brief Null output strategy.
///
auto null_output(const FrameSyncProcess::AudioHop &frame) -> void;
