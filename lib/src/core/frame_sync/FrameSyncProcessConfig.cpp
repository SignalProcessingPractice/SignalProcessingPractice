///
/// @file FrameSyncProcessConfig.cpp
///
#include "FrameSyncProcessConfig.hpp"

#include "Strategies/FFT.hpp"
#include "Strategies/IFFT.hpp"
#include "Strategies/Overlapper.hpp"
#include "Strategies/RectangleOverlapAdder.hpp"
#include "Strategies/RectangleWindow.hpp"
#include "Strategies/null_strategies.hpp"

auto get_default_null_input_strategy() -> FrameSyncProcess::AudioAcquireStrategy {
    static NullInput instance;
    static FrameSyncProcess::AudioAcquireStrategy slot{&instance};
    return slot;
}

auto get_default_through_preprocess_strategy() -> FrameSyncProcess::PreProcessStrategy {
    static ThroughPreProcess instance;
    static FrameSyncProcess::PreProcessStrategy slot{&instance};
    return slot;
}

auto get_default_overlapper_strategy() -> FrameSyncProcess::OverlapStrategy {
    static Overlapper instance;
    static FrameSyncProcess::OverlapStrategy slot{&instance};
    return slot;
}

auto get_default_rectangle_window_strategy() -> FrameSyncProcess::WindowStrategy {
    static RectangleWindow instance;
    static FrameSyncProcess::WindowStrategy slot{&instance};
    return slot;
}

auto get_default_fft_strategy() -> FrameSyncProcess::FftStrategy {
    static FFT instance;
    static FrameSyncProcess::FftStrategy slot{&instance};
    return slot;
}

auto get_default_through_infer_strategy() -> FrameSyncProcess::InferStrategy {
    static ThroughInfer instance;
    static FrameSyncProcess::InferStrategy slot{&instance};
    return slot;
}

auto get_default_ifft_postprocess_strategy() -> FrameSyncProcess::PostProcessStrategy {
    static IFFT instance;
    static FrameSyncProcess::PostProcessStrategy slot{&instance};
    return slot;
}

auto get_default_rectangle_overlap_adder_strategy() -> FrameSyncProcess::OverlapAddStrategy {
    static RectangleOverlapAdder instance;
    static FrameSyncProcess::OverlapAddStrategy slot{&instance};
    return slot;
}

auto get_default_null_output_strategy() -> FrameSyncProcess::AudioOutputStrategy {
    static NullOutput instance;
    static FrameSyncProcess::AudioOutputStrategy slot{&instance};
    return slot;
}
