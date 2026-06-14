///
/// @file RectangleOverlapAdder.cpp
///
#include "Strategies/RectangleOverlapAdder.hpp"

#include "FrameSyncProcess.hpp"
#include <algorithm>
#include <utility>

///
/// @todo 加算時の係数は, コンストラクタで設定可能にする.
///

FrameSyncProcess::AudioHop
    RectangleOverlapAdder::Execute(
        FrameSyncProcess::AudioFrame&& frame
    )
{
    constexpr auto hop =
        FrameSyncProcess::audio_hop_length;

    // frame の先頭部と前フレームの末尾部を加算する.
    std::transform(
        frame.begin(),
        frame.begin() + hop,
        frame_buffer_.end() - hop,
        hop_buffer_.begin(),
        [](float curr, float prev) { return curr + prev; });

    // 次のフレームのために frame_buffer_ を更新する.
    frame_buffer_ = std::move(frame);

    return hop_buffer_;
}
