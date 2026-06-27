///
/// @file RectangleOverlapAdder.cpp
///
#include "Strategies/RectangleOverlapAdder.hpp"

#include <algorithm>
#include <iterator>

#include "FrameSyncProcess.hpp"

///
/// @todo 加算時の係数は, コンストラクタで設定可能にする.
///

auto RectangleOverlapAdder::Execute(const FrameSyncProcess::AudioFrame &frame)
        -> FrameSyncProcess::AudioHop {
    constexpr auto kHopLength = static_cast<std::ptrdiff_t>(FrameSyncProcess::audio_hop_length);

    // frame の先頭部と前フレームの末尾部を加算する.
    std::transform(frame.begin(), std::next(frame.begin(), kHopLength),
                   std::prev(frame_buffer_.end(), kHopLength), hop_buffer_.begin(),
                   [](float curr, float prev) {
                       return curr + prev;
                   });

    // 次のフレームのために frame_buffer_ を更新する.
    frame_buffer_ = frame;

    return hop_buffer_;
}
