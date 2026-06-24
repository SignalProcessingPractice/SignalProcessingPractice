///
/// @file FFT.hpp
///
#pragma once

#include "FrameSyncProcess.hpp"
#include "arm_math.h"

class FFT {
public:
    FFT();

    FrameSyncProcess::AudioFrame Execute(FrameSyncProcess::AudioFrame &&frame);

private:
    FrameSyncProcess::AudioFrame frame_buffer_;

    // ARM CMSIS FFT instance and init flag
    arm_rfft_fast_instance_f32 rfft_instance_{};
    bool initialized_ = false;
};
