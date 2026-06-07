///
/// @file PipelineContextConfig.hpp
///
#pragma once

#include "FrameSyncProcess.hpp"

#include <etl/delegate.h>

#include "Strategies/null_strategies.hpp"
#include "Strategies/Overlapper.hpp"
#include "Strategies/HannWindow.hpp"
#include "Strategies/FFT.hpp"
#include "Strategies/OverlapAdder.hpp"

struct PipelineContextConfig
{
    FrameSyncProcess::AudioAquireStrategy     audio_aquire_strategy    = null_input;
    FrameSyncProcess::PreProcessStrategy      pre_process_strategy     = null_preprocess;
    FrameSyncProcess::OverlapStrategy         overlap_strategy         = default_overlapper_delegate_;

    FrameSyncProcess::WindowStrategy          window_strategy          = default_window_delegate_;
    FrameSyncProcess::FftStrategy             fft_strateg              = default_fft_delegate_;
    FrameSyncProcess::InferStrategy           infer_strategy           = null_infer;

    FrameSyncProcess::PostProcessStrategy     post_process_strategy    = null_postprocess;
    FrameSyncProcess::OverlapAddStrategy      overlap_add_strategy     = default_overlap_adder_delegate_;
    FrameSyncProcess::AudioOutputStrategy     audio_output_strategy    = null_output;

private:

    ///
    /// ステートフルな Strategy をデフォルト設定値として使用する場合は, 本 Parameter Object にてメンバとして定義しておく方針とする.
    ///
    Overlapper default_overlapper_;
    FrameSyncProcess::OverlapStrategy default_overlapper_delegate_ =
        FrameSyncProcess::OverlapStrategy
            ::create<Overlapper, &Overlapper::Execute>(default_overlapper_);
    
    HannWindow default_window_;
    FrameSyncProcess::WindowStrategy default_window_delegate_ =
        FrameSyncProcess::WindowStrategy
            ::create<HannWindow, &HannWindow::Execute>(default_window_);
    
    FFT default_fft_;
    FrameSyncProcess::FftStrategy default_fft_delegate_ =
        FrameSyncProcess::FftStrategy
            ::create<FFT, &FFT::Execute>(default_fft_);

    OverlapAdder default_overlap_adder_;
    FrameSyncProcess::OverlapAddStrategy default_overlap_adder_delegate_ =
        FrameSyncProcess::OverlapAddStrategy
            ::create<OverlapAdder, &OverlapAdder::Execute>(default_overlap_adder_);
};
