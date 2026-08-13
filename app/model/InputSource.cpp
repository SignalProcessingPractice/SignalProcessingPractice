///
/// @file InputSource.cpp
///

#include "model/InputSource.h"

#include <chrono>
#include <span>
#include <utility>

#include "common/AudioConfig.h"
#include "model/AudioInputBuffer.h"

InputSource::InputSource(AudioInputBuffer* buffer)
    : buffer_(buffer),
      generator_([] {
          return FrameSyncProcess::AudioHop{kAppSampleRate};
      })
{
}

void InputSource::SetGenerator(HopGenerator generator)
{
    const std::lock_guard<std::mutex> guard{generator_mutex_};
    generator_ = std::move(generator);
}

void InputSource::RunWithGeneratorLock(const std::function<void()>& func)
{
    const std::lock_guard<std::mutex> guard{generator_mutex_};
    func();
}

void InputSource::Start()
{
    if (thread_.joinable()) {
        return;
    }
    thread_ = std::jthread{[this](const std::stop_token& stop_token) {
        Run(stop_token);
    }};
}

void InputSource::Stop()
{
    if (thread_.joinable()) {
        thread_.request_stop();
        thread_.join();
    }
}

void InputSource::Run(const std::stop_token& stop_token)
{
    auto next_deadline = std::chrono::steady_clock::now() + kHopPeriod;
    while (!stop_token.stop_requested()) {
        const auto hop = GenerateHop();
        buffer_->Push(std::span<const float>{hop.data(), hop.size()});
        std::this_thread::sleep_until(next_deadline);
        next_deadline += kHopPeriod;
    }
}

auto InputSource::GenerateHop() -> FrameSyncProcess::AudioHop
{
    const std::lock_guard<std::mutex> guard{generator_mutex_};
    auto hop = generator_();
    hop.set_sample_rate(kAppSampleRate);
    return hop;
}
