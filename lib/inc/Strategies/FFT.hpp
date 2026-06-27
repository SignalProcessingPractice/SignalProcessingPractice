///
/// @file FFT.hpp
///
#pragma once

#include "FrameSyncProcess.hpp"
#include "arm_math.h"

class FFT {
public:
    FFT();

    auto Execute(const FrameSyncProcess::AudioFrame &frame) -> FrameSyncProcess::AudioFrame;

private:
    FrameSyncProcess::AudioFrame input_buffer_;
    FrameSyncProcess::AudioFrame frame_buffer_;

    // ARM CMSIS FFT instance and init flag
    arm_rfft_fast_instance_f32 rfft_instance_{};
    bool initialized_ = false;
};
