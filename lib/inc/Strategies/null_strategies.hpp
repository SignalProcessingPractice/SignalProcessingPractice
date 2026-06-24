///
/// @file null_strategy.hpp
///
#pragma once

#include "FrameSyncProcess.hpp"

///
/// @brief Null input strategy.
///
FrameSyncProcess::AudioHop null_input(void);

///
/// @brief Null preprocessing strategy.
///
FrameSyncProcess::AudioHop through_preprocess(FrameSyncProcess::AudioHop &&frame);

///
/// @brief Null postprocessing strategy.
///
FrameSyncProcess::AudioFrame through_postprocess(FrameSyncProcess::AudioFrame &&frame);

///
/// @brief Null Window strategy.
///
FrameSyncProcess::AudioFrame null_window(FrameSyncProcess::AudioFrame &&frame);

///
/// @brief Through Infer strategy.
///
FrameSyncProcess::AudioFrame through_infer(FrameSyncProcess::AudioFrame &&frame);

///
/// @brief Null Infer strategy.
///
FrameSyncProcess::AudioFrame null_infer(FrameSyncProcess::AudioFrame &&frame);

///
/// @brief Null output strategy.
///
void null_output(FrameSyncProcess::AudioHop &&frame);
