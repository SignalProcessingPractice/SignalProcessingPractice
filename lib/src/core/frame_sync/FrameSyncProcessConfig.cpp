///
/// @file FrameSyncProcessConfig.cpp
///
#include "FrameSyncProcessConfig.hpp"

#include "Strategies/Overlapper.hpp"
#include "Strategies/RectangleWindow.hpp"
#include "Strategies/FFT.hpp"
#include "Strategies/IFFT.hpp"
#include "Strategies/RectangleOverlapAdder.hpp"

///
/// デフォルト Strategy の定義.
///

Overlapper default_overlapper_;
FrameSyncProcess::OverlapStrategy default_overlapper_delegate_ =
    FrameSyncProcess::OverlapStrategy
        ::create<Overlapper, &Overlapper::Execute>(default_overlapper_);

RectangleWindow default_rectangle_window_;
FrameSyncProcess::WindowStrategy default_rectangle_window_delegate_ =
    FrameSyncProcess::WindowStrategy
        ::create<RectangleWindow, &RectangleWindow::Execute>(default_rectangle_window_);

FFT default_fft_;
FrameSyncProcess::FftStrategy default_fft_delegate_ =
    FrameSyncProcess::FftStrategy
        ::create<FFT, &FFT::Execute>(default_fft_);

IFFT default_postprocess_ifft_;
FrameSyncProcess::FftStrategy default_ifft_postprocess_delegate_ =
    FrameSyncProcess::FftStrategy
        ::create<IFFT, &IFFT::Execute>(default_postprocess_ifft_);

RectangleOverlapAdder default_rectangle_overlap_adder_;
FrameSyncProcess::OverlapAddStrategy default_rectangle_overlap_adder_delegate_ =
    FrameSyncProcess::OverlapAddStrategy
        ::create<RectangleOverlapAdder, &RectangleOverlapAdder::Execute>(default_rectangle_overlap_adder_);
