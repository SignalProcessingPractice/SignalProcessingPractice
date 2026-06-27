///
/// @file SineGenerator.cpp
///
#include "Strategies/SineGenerator.hpp"

#include <cmath>
#include <numbers>
#include <span>

SineGenerator::SineGenerator(Params params)
    : frequency_(params.frequency), amplitude_(params.amplitude) {
}

auto SineGenerator::GenerateOneHop() -> FrameSyncProcess::AudioHop {
    FrameSyncProcess::AudioHop hop_frame;

    const double sample_rate = hop_frame.sample_rate();
    static constexpr double kTwoPi = 2.0 * std::numbers::pi;
    const double phase_increment = kTwoPi * frequency_ / sample_rate;

    const auto hop_span = std::span<float>(hop_frame.data(), hop_frame.size());

    for (std::size_t i = 0; i < hop_frame.size(); ++i) {
        hop_span[i] = static_cast<float>(amplitude_ * std::sin(phase_));

        phase_ += phase_increment;

        if (phase_ >= kTwoPi) {
            phase_ -= kTwoPi;
        }
    }

    return hop_frame;
}

void SineGenerator::SetFrequency(double frequency) {
    frequency_ = frequency;
}

void SineGenerator::SetAmplitude(double amplitude) {
    amplitude_ = amplitude;
}
