///
/// @file RingBufferOutput.cpp
///

#include "model/RingBufferOutput.h"

#include <span>

#include "model/AudioOutputBuffer.h"

RingBufferOutput::RingBufferOutput(AudioOutputBuffer* buffer) : buffer_(buffer) {
}

auto RingBufferOutput::Exec(const FrameSyncProcess::AudioHop& hop) -> void {
    // 満杯時は破棄する方針 (Push が false を返すのみ).
    buffer_->Push(std::span<const float>{hop.data(), hop.size()});
}

auto RingBufferOutput::Reset() -> void {
    buffer_->Clear();
}
