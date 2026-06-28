///
/// @file FrameSyncProcessConfig.hpp
///
#pragma once

#include "FrameSyncProcess.hpp"

///
/// @brief デフォルト Strategy の取得関数.
///
auto get_default_null_input_strategy() -> FrameSyncProcess::AudioAquireStrategy;
auto get_default_through_preprocess_strategy() -> FrameSyncProcess::PreProcessStrategy;
auto get_default_overlapper_strategy() -> FrameSyncProcess::OverlapStrategy;
auto get_default_rectangle_window_strategy() -> FrameSyncProcess::WindowStrategy;
auto get_default_fft_strategy() -> FrameSyncProcess::FftStrategy;
auto get_default_through_infer_strategy() -> FrameSyncProcess::InferStrategy;
auto get_default_ifft_postprocess_strategy() -> FrameSyncProcess::PostProcessStrategy;
auto get_default_rectangle_overlap_adder_strategy() -> FrameSyncProcess::OverlapAddStrategy;
auto get_default_null_output_strategy() -> FrameSyncProcess::AudioOutputStrategy;

struct FrameSyncProcessConfig {
public:
    FrameSyncProcess::AudioAquireStrategy audio_aquire_strategy =
            get_default_null_input_strategy();
    FrameSyncProcess::PreProcessStrategy pre_process_strategy =
            get_default_through_preprocess_strategy();
    FrameSyncProcess::OverlapStrategy overlap_strategy = get_default_overlapper_strategy();

    FrameSyncProcess::WindowStrategy window_strategy = get_default_rectangle_window_strategy();
    FrameSyncProcess::FftStrategy fft_strategy = get_default_fft_strategy();
    FrameSyncProcess::InferStrategy infer_strategy = get_default_through_infer_strategy();

    FrameSyncProcess::PostProcessStrategy post_process_strategy =
            get_default_ifft_postprocess_strategy();
    FrameSyncProcess::OverlapAddStrategy overlap_add_strategy =
            get_default_rectangle_overlap_adder_strategy();
    FrameSyncProcess::AudioOutputStrategy audio_output_strategy =
            get_default_null_output_strategy();
};
