///
/// @file HannOverlapAdder.cpp
///
#include "common/Strategies/HannOverlapAdder.hpp"

#include <algorithm>
#include <iterator>

#include "arm_math.h"
#include "common/FrameSyncProcess.hpp"

HannOverlapAdder::HannOverlapAdder()
{
    constexpr auto kFrameLength = static_cast<uint32_t>(FrameSyncProcess::audio_frame_length);

    arm_hanning_f32(window_.data(), kFrameLength);
}

auto HannOverlapAdder::Exec(const FrameSyncProcess::AudioFrame& frame) -> FrameSyncProcess::AudioHop
{
    constexpr auto kHopLength = static_cast<std::ptrdiff_t>(FrameSyncProcess::audio_hop_length);
    constexpr auto kFrameLength = static_cast<uint32_t>(FrameSyncProcess::audio_frame_length);

    arm_mult_f32(frame.data(), window_.data(), windowed_buffer_.data(), kFrameLength);

    std::transform(windowed_buffer_.begin(), std::next(windowed_buffer_.begin(), kHopLength),
                   std::prev(frame_buffer_.end(), kHopLength), hop_buffer_.begin(),
                   [](float curr, float prev) {
                       return curr + prev;
                   });

    // 次のフレームのために frame_buffer_ を更新する.
    frame_buffer_ = windowed_buffer_;

    return hop_buffer_;
}

auto HannOverlapAdder::Reset() -> void
{
    hop_buffer_ = FrameSyncProcess::AudioHop{};
    windowed_buffer_ = FrameSyncProcess::AudioFrame{};
    frame_buffer_ = FrameSyncProcess::AudioFrame{};
}
