///
/// @file HannWindow.cpp
///
#include "Strategies/HannWindow.hpp"

#include "arm_math.h"

HannWindow::HannWindow()
{
    constexpr auto N =
        FrameSyncProcess::audio_frame_length;

    arm_hanning_f32( window_.data(), N);
}

FrameSyncProcess::AudioFrame
    HannWindow::Execute(
        FrameSyncProcess::AudioFrame&& frame)
{
    constexpr auto N =
        FrameSyncProcess::audio_frame_length;

    arm_mult_f32(
        frame.data(),
        window_.data(),
        frame_buffer_.data(),
        N);

    return frame_buffer_;
}
