///
/// @file HannWindow.cpp
///
#include "Strategies/HannWindow.hpp"

#include "arm_math.h"

HannWindow::HannWindow() {
    constexpr auto kFrameLength = static_cast<uint32_t>(FrameSyncProcess::audio_frame_length);

    arm_hanning_f32(window_.data(), kFrameLength);
}

auto HannWindow::Execute(const FrameSyncProcess::AudioFrame &frame) -> FrameSyncProcess::AudioFrame {
    constexpr auto kFrameLength = static_cast<uint32_t>(FrameSyncProcess::audio_frame_length);

    arm_mult_f32(frame.data(), window_.data(), frame_buffer_.data(), kFrameLength);

    return frame_buffer_;
}
