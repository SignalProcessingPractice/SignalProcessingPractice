///
/// @file RectangleOverlapAdder.cpp
///
#include "Strategies/RectangleOverlapAdder.hpp"

#include "FrameSyncProcess.hpp"
#include <algorithm>
#include <utility>

FrameSyncProcess::AudioHop
    RectangleOverlapAdder::Execute(
        FrameSyncProcess::AudioFrame&& frame
    )
{
    constexpr auto hop =
        FrameSyncProcess::audio_hop_length;

    const auto tail_start = frame.size() - hop;

    // frame の末端と frame_buffer_ の先端をそれぞれ 0.5 倍して加算することで矩形波窓を実現する.
    std::transform(
        frame.begin() + tail_start,
        frame.end(),
        frame_buffer_.begin(),
        hop_buffer_.begin(),
        [](float curr, float prev) { return curr * 0.5f + prev * 0.5f; });

    // 次のフレームのために frame_buffer_ を更新する.
    frame_buffer_ = std::move(frame);

    return hop_buffer_;
}
