///
/// @file null_strategies.cpp
///
#include "Strategies/null_strategies.hpp"

#include "FrameSyncProcess.hpp"

auto null_input() -> FrameSyncProcess::AudioHop {
    return FrameSyncProcess::AudioHop{};
}

auto through_preprocess(const FrameSyncProcess::AudioHop &frame) -> FrameSyncProcess::AudioHop {
    return frame;
}

auto through_postprocess(const FrameSyncProcess::AudioFrame &frame)
        -> FrameSyncProcess::AudioFrame {
    return frame;
}

auto null_overlap([[maybe_unused]] const FrameSyncProcess::AudioHop &frame)
        -> FrameSyncProcess::AudioFrame {
    return FrameSyncProcess::AudioFrame{};
}

auto null_window(const FrameSyncProcess::AudioFrame &frame) -> FrameSyncProcess::AudioFrame {
    return frame;
}

auto null_fft(const FrameSyncProcess::AudioFrame &frame) -> FrameSyncProcess::AudioFrame {
    return frame;
}

auto through_infer(const FrameSyncProcess::AudioFrame &frame) -> FrameSyncProcess::AudioFrame {
    return frame;
}

auto null_infer([[maybe_unused]] const FrameSyncProcess::AudioFrame &frame)
        -> FrameSyncProcess::AudioFrame {
    return FrameSyncProcess::AudioFrame{};
}

auto null_overlap_add([[maybe_unused]] const FrameSyncProcess::AudioFrame &frame)
        -> FrameSyncProcess::AudioHop {
    return FrameSyncProcess::AudioHop{};
}

auto null_output([[maybe_unused]] const FrameSyncProcess::AudioHop &frame) -> void {
}
