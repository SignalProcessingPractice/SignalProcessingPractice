///
/// @file AudioConfig.h
///
#pragma once

#include <chrono>
#include <cstdint>

#include "FrameSyncProcess.hpp"

///
/// アプリケーションのサンプルレート.
///
inline constexpr std::uint32_t kAppSampleRate = 44100U;

///
/// 1 ホップ分の周期 (512 / 44100 Hz ≒ 11.61 ms).
///
inline constexpr auto kHopPeriod = std::chrono::nanoseconds{
        std::chrono::nanoseconds{std::chrono::seconds{1}}.count() *
        static_cast<std::int64_t>(FrameSyncProcess::audio_hop_length) / kAppSampleRate};
