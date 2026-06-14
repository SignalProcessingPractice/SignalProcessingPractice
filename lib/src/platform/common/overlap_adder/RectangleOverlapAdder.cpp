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

    std::fill(
        hop_buffer_.begin(),
        hop_buffer_.end(),
        0.0f);

    // frame の末端と frame_buffer_ の先端をそれぞれ 0.5 倍して加算することで矩形波窓を実現する.
    {
        for (std::size_t i = (frame.size() - hop); i < frame.size(); ++i)
        {
            hop_buffer_[i] = 0.5f * frame[i];
        }

        for (std::size_t i = 0; i < hop; ++i)
        {
            hop_buffer_[i] += 0.5f * frame_buffer_[i];
        }
    }

    // 次のフレームのために frame_buffer_ を更新する.
    frame_buffer_ = std::move(frame);

    return hop_buffer_;
}
