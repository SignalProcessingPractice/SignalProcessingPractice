///
/// @file FFT.hpp
///
#pragma once

#include "FrameSyncProcess.hpp"
#include "arm_math.h"

class IFFT {
public:
    IFFT();

    auto Exec(const FrameSyncProcess::AudioFrame& frame) -> FrameSyncProcess::AudioFrame;
    auto Reset() -> void;

private:
    FrameSyncProcess::AudioFrame input_buffer_;
    FrameSyncProcess::AudioFrame frame_buffer_;

    arm_rfft_fast_instance_f32 rfft_instance_{};
    bool initialized_ = false;
};
