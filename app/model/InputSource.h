///
/// @file InputSource.h
///
#pragma once

#include <functional>
#include <mutex>
#include <stop_token>
#include <thread>

#include "FrameSyncProcess.hpp"

class AudioInputBuffer;

///
/// @brief 入力サンプルの Producer スレッド.
///
/// ホップ周期ごとに generator で 1 ホップ生成し, AudioInputBuffer へ push する.
/// デバイスのクロックを模擬するため, 絶対時刻基準の sleep_until で周期を刻む.
/// generator の差し替えによって入力種別 (無音 / Sine 等) を切り替える.
///
class InputSource {
public:
    using HopGenerator = std::function<FrameSyncProcess::AudioHop()>;

    explicit InputSource(AudioInputBuffer* buffer);

    ///
    /// 生成関数を差し替える (スレッド動作中も可).
    ///
    void SetGenerator(HopGenerator generator);

    void Start();
    void Stop();

private:
    void Run(const std::stop_token& stop_token);
    auto GenerateHop() -> FrameSyncProcess::AudioHop;

    AudioInputBuffer* buffer_;

    std::mutex generator_mutex_;
    HopGenerator generator_;

    std::jthread thread_;
};
