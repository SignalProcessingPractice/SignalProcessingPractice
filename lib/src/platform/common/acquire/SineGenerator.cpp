///
/// @file SineGenerator.cpp
///
#include <cmath>
#include <numbers>

#include "Strategies/SineGenerator.hpp"

SineGenerator::SineGenerator(
    double frequency,
    double amplitude
)
    : frequency_(frequency)
    , amplitude_(amplitude)
{
}

FrameSyncProcess::AudioFrame 
    SineGenerator::GenerateOneFrame(
        void
    )
{
    FrameSyncProcess::AudioFrame frame;

    const double sample_rate = frame.sample_rate();

    // 1サンプルあたりの位相増分
    const double phase_increment =
        2.0 * std::numbers::pi * frequency_ / sample_rate;

    auto* data = frame.data();

    for (std::size_t i = 0; i < frame.size(); ++i)
    {
        data[i] = amplitude_ * std::sin(phase_);

        phase_ += phase_increment;

        // 位相を 0〜2π に正規化
        if (phase_ >= 2.0 * std::numbers::pi)
        {
            phase_ -= 2.0 * std::numbers::pi;
        }
    }

    return frame;
}

void SineGenerator::SetFrequency(double frequency)
{
    frequency_ = frequency;
}

void SineGenerator::SetAmplitude(double amplitude)
{
    amplitude_ = amplitude;
}
