///
/// @file Overlapper.cpp
///
#include "Strategies/Overlapper.hpp"

#include "FrameSyncProcess.hpp"

FrameSyncProcess::AudioFrame
    Overlapper::Execute(
        FrameSyncProcess::AudioHop&& frame
    )
{
    constexpr auto hop =
        FrameSyncProcess::audio_hop_length;

    std::move(
        frame_buffer_.begin() + hop,
        frame_buffer_.end(),
        frame_buffer_.begin());

    std::copy(
        frame.begin(),
        frame.end(),
        frame_buffer_.end() - hop);

    return frame_buffer_;
}
