///
/// @file SineGenerator.hpp
///
#pragma once

#include "FrameSyncProcess.hpp"

class SineGenerator {
public:
    SineGenerator(double frequency = 440.0, double amplitude = 0.5);

    FrameSyncProcess::AudioHop GenerateOneHop();

    void SetFrequency(double frequency);

    void SetAmplitude(double amplitude);

private:
    double frequency_;
    double amplitude_;
    double phase_ = 0.0;
};
