///
/// @file RingBufferAcquire.cpp
///
#include "Strategies/RingBufferAcquire.hpp"

auto RingBufferAcquire::Exec() -> FrameSyncProcess::AudioHop
{
    FrameSyncProcess::AudioHop hop;
    model_->pop_hop(&hop);
    return hop;
}

auto RingBufferAcquire::Reset() -> void
{
    model_->clear();
}

auto RingBufferAcquire::Push(std::span<const float> samples) -> bool
{
    return model_->push(samples);
}

auto RingBufferAcquire::WaitForHop(std::stop_token stop_token) -> bool
{
    return model_->wait_for_hop(std::move(stop_token));
}
