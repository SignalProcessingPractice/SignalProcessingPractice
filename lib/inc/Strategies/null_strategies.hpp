///
/// @file null_strategy.hpp
///
#pragma once

#include "FrameSyncProcess.hpp"

///
/// @brief Null input strategy.
///
FrameSyncProcess::AudioHop
    null_input (
        void
    );

///
/// @brief Null preprocessing strategy.
///
FrameSyncProcess::AudioHop
    null_preprocess (
        FrameSyncProcess::AudioHop &&frame
    );

///
/// @brief Null postprocessing strategy.
///
FrameSyncProcess::AudioFrame
    null_postprocess (
        FrameSyncProcess::AudioFrame &&frame
    );

///
/// @brief Null output strategy.
///
void
    null_output (
        FrameSyncProcess::AudioHop &&frame
    );
