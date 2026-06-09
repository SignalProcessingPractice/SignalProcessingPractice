///
/// @file FrameSyncProcessConfig.cpp
///
#include "FrameSyncProcessConfig.hpp"

#include "Strategies/Overlapper.hpp"
#include "Strategies/HannWindow.hpp"
#include "Strategies/FFT.hpp"
#include "Strategies/OverlapAdder.hpp"

///
/// デフォルト Strategy の定義.
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
