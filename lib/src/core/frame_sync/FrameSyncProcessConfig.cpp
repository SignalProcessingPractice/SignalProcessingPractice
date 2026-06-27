///
/// @file FrameSyncProcessConfig.cpp
///
#include "FrameSyncProcessConfig.hpp"

#include "Strategies/FFT.hpp"
#include "Strategies/IFFT.hpp"
#include "Strategies/Overlapper.hpp"
#include "Strategies/RectangleOverlapAdder.hpp"
#include "Strategies/RectangleWindow.hpp"

auto get_default_overlapper_delegate() -> const FrameSyncProcess::OverlapStrategy& {
    static Overlapper instance;
    static const auto delegate =
            FrameSyncProcess::OverlapStrategy::create<Overlapper, &Overlapper::Execute>(instance);
    return delegate;
}

auto get_default_rectangle_window_delegate() -> const FrameSyncProcess::WindowStrategy& {
    static RectangleWindow instance;
    static const auto delegate =
            FrameSyncProcess::WindowStrategy::create<RectangleWindow, &RectangleWindow::Execute>(
                    instance);
    return delegate;
}

auto get_default_fft_delegate() -> const FrameSyncProcess::FftStrategy& {
    static FFT instance;
    static const auto delegate =
            FrameSyncProcess::FftStrategy::create<FFT, &FFT::Execute>(instance);
    return delegate;
}

auto get_default_ifft_postprocess_delegate() -> const FrameSyncProcess::FftStrategy& {
    static IFFT instance;
    static const auto delegate =
            FrameSyncProcess::FftStrategy::create<IFFT, &IFFT::Execute>(instance);
    return delegate;
}

auto get_default_rectangle_overlap_adder_delegate() -> const FrameSyncProcess::OverlapAddStrategy& {
    static RectangleOverlapAdder instance;
    static const auto delegate =
            FrameSyncProcess::OverlapAddStrategy::create<RectangleOverlapAdder,
                                                         &RectangleOverlapAdder::Execute>(instance);
    return delegate;
}
