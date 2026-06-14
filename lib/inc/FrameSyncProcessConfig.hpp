///
/// @file FrameSyncProcessConfig.hpp
///
#pragma once

#include <etl/delegate.h>

#include "Strategies/null_strategies.hpp"

///
/// デフォルト Strategy の前方宣言.
///
extern FrameSyncProcess::OverlapStrategy    default_overlapper_delegate_;
extern FrameSyncProcess::WindowStrategy     default_window_delegate_;
extern FrameSyncProcess::FftStrategy        default_fft_delegate_;
extern FrameSyncProcess::OverlapAddStrategy default_overlap_adder_delegate_;


struct FrameSyncProcessConfig
{
public:
    FrameSyncProcess::AudioAquireStrategy     audio_aquire_strategy    = null_input;
    FrameSyncProcess::PreProcessStrategy      pre_process_strategy     = through_preprocess;
    FrameSyncProcess::OverlapStrategy         overlap_strategy         = default_overlapper_delegate_;

    FrameSyncProcess::WindowStrategy          window_strategy          = default_window_delegate_;
    FrameSyncProcess::FftStrategy             fft_strategy             = default_fft_delegate_;
    FrameSyncProcess::InferStrategy           infer_strategy           = through_infer;

    FrameSyncProcess::PostProcessStrategy     post_process_strategy    = through_postprocess;
    FrameSyncProcess::OverlapAddStrategy      overlap_add_strategy     = default_overlap_adder_delegate_;
    FrameSyncProcess::AudioOutputStrategy     audio_output_strategy    = null_output;

};
