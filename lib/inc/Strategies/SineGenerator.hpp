///
/// @file SineGenerator.hpp
///
#pragma once

#include "FrameSyncProcess.hpp"

class SineGenerator {
public:
    static constexpr double kDefaultFrequency = 440.0;
    static constexpr double kDefaultAmplitude = 0.5;

    struct Params {
        double frequency;
        double amplitude;
    };

    explicit SineGenerator(Params params);

    auto GenerateOneHop() -> FrameSyncProcess::AudioHop;

    void SetFrequency(double frequency);

    void SetAmplitude(double amplitude);

private:
    double frequency_;
    double amplitude_;
    double phase_ = 0.0;
};
