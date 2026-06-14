///
/// @file HannOverlapAdder.cpp
///
#include "Strategies/HannOverlapAdder.hpp"

#include <algorithm>
#include <utility>

#include "arm_math.h"

#include "FrameSyncProcess.hpp"

HannOverlapAdder::HannOverlapAdder()
{
    constexpr auto N =
        FrameSyncProcess::audio_frame_length;

    arm_hanning_f32( window_.data(), N);
}

FrameSyncProcess::AudioHop
    HannOverlapAdder::Execute(
        FrameSyncProcess::AudioFrame&& frame
    )
{
    constexpr auto hop =
        FrameSyncProcess::audio_hop_length;

    constexpr auto N =
        FrameSyncProcess::audio_frame_length;

    arm_mult_f32(
        frame.data(),
        window_.data(),
        windowed_buffer_.data(),
        N);

    const auto tail_start = frame.size() - hop;
    std::transform(
        windowed_buffer_.begin() + tail_start,
        windowed_buffer_.end(),
        frame_buffer_.begin(),
        hop_buffer_.begin(),
        [](float curr, float prev) { return curr + prev; });

    // 次のフレームのために frame_buffer_ を更新する.
    frame_buffer_ = std::move(windowed_buffer_);

    return hop_buffer_;
}
