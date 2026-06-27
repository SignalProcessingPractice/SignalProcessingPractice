///
/// @file FrameSyncProcessConfig.hpp
///
#pragma once

#include <etl/delegate.h>

#include "Strategies/null_strategies.hpp"

///
/// デフォルト Strategy の取得関数.
///
auto get_default_overlapper_delegate() -> const FrameSyncProcess::OverlapStrategy &;
auto get_default_rectangle_window_delegate() -> const FrameSyncProcess::WindowStrategy &;
auto get_default_fft_delegate() -> const FrameSyncProcess::FftStrategy &;
auto get_default_ifft_postprocess_delegate() -> const FrameSyncProcess::FftStrategy &;
auto get_default_rectangle_overlap_adder_delegate() -> const FrameSyncProcess::OverlapAddStrategy &;

struct FrameSyncProcessConfig {
public:
    FrameSyncProcess::AudioAquireStrategy audio_aquire_strategy = null_input;
    FrameSyncProcess::PreProcessStrategy pre_process_strategy = through_preprocess;
    FrameSyncProcess::OverlapStrategy overlap_strategy = get_default_overlapper_delegate();

    FrameSyncProcess::WindowStrategy window_strategy = get_default_rectangle_window_delegate();
    FrameSyncProcess::FftStrategy fft_strategy = get_default_fft_delegate();
    FrameSyncProcess::InferStrategy infer_strategy = through_infer;

    FrameSyncProcess::PostProcessStrategy post_process_strategy =
            get_default_ifft_postprocess_delegate();
    FrameSyncProcess::OverlapAddStrategy overlap_add_strategy =
            get_default_rectangle_overlap_adder_delegate();
    FrameSyncProcess::AudioOutputStrategy audio_output_strategy = null_output;
};
