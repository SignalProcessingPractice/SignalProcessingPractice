///
/// @file RingBufferAcquire.cpp
///

#include "model/RingBufferAcquire.h"

#include "model/AudioConfig.h"
#include "model/AudioInputBuffer.h"

RingBufferAcquire::RingBufferAcquire(AudioInputBuffer* buffer) : buffer_(buffer) {
}

auto RingBufferAcquire::Exec() -> FrameSyncProcess::AudioHop {
    FrameSyncProcess::AudioHop hop{kAppSampleRate};
    buffer_->PopHop(&hop);
    return hop;
}

auto RingBufferAcquire::Reset() -> void {
    buffer_->Clear();
}
