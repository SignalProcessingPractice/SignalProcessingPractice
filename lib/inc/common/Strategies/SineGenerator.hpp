///
/// @file SineGenerator.hpp
///
#pragma once

#include "common/FrameSyncProcess.hpp"

class SineGenerator {
public:
    static constexpr double kDefaultFrequency = 440.0;
    static constexpr double kDefaultAmplitude = 0.5;

    struct Params {
        double frequency;
        double amplitude;
    };

    explicit SineGenerator(Params params);

    auto Exec() -> FrameSyncProcess::AudioHop;
    auto Reset() -> void;

    auto SetFrequency(double frequency) -> void;
    auto SetAmplitude(double amplitude) -> void;

private:
    double frequency_;
    double amplitude_;
    double phase_ = 0.0;
};
