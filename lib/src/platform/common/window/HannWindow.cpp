///
/// @file HannWindow.cpp
///
#include "Strategies/HannWindow.hpp"

#include "arm_math.h"

HannWindow::HannWindow()
{
    constexpr auto N =
        FrameSyncProcess::audio_frame_length;

    for (uint32_t i = 0; i < N; ++i)
    {
        window_[i] =
            0.5f *
            (1.0f -
             arm_cos_f32(
                 2.0f * PI * static_cast<float>(i) /
                 static_cast<float>(N - 1)));
    }
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
