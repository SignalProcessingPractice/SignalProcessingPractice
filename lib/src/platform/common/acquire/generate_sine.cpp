///
/// @file generate_sine.cpp
///
#include <numbers>
#include <cmath>

#include "FrameSyncProcess.hpp"

FrameSyncProcess::AudioFrame generate_sine(double& phase)
{
    FrameSyncProcess::AudioFrame frame;

    const double frequency   = 440.0;   // A4 音の周波数
    const double sample_rate = frame.sample_rate();
    const double amplitude   = 0.5;     // 振幅

    // 1サンプルあたりの位相増分
    const double phase_increment =
        2.0 * std::numbers::pi * frequency / sample_rate;

    auto* data = frame.data();

    for (std::size_t i = 0; i < frame.size(); ++i)
    {
        data[i] = amplitude * std::sin(phase);

        phase += phase_increment;

        // 位相を 0〜2π に正規化（オーバーフロー防止）
        if (phase >= 2.0 * std::numbers::pi)
        {
            phase -= 2.0 * std::numbers::pi;
        }
    }

    return frame;
}
