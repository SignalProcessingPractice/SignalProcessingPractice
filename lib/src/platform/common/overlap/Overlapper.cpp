///
/// @file Overlapper.cpp
///
#include "Strategies/Overlapper.hpp"

#include "FrameSyncProcess.hpp"

FrameSyncProcess::AudioFrame
    Overlapper::Execute(
        FrameSyncProcess::AudioHop&& hop
    )
{
    constexpr auto hop_size =
        FrameSyncProcess::audio_hop_length;

    std::move(
        frame_buffer_.begin() + hop_size,
        frame_buffer_.end(),
        frame_buffer_.begin());

    std::copy(
        hop.begin(),
        hop.end(),
        frame_buffer_.end() - hop_size);

    return frame_buffer_;
}
