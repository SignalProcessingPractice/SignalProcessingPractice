///
/// @file IFFT.cpp
///
#include "Strategies/IFFT.hpp"

#include "arm_math.h"

IFFT::IFFT() {
    // コンストラクタで IFFT インスタンスを初期化する（固定サイズを想定）
    const auto fftSize = static_cast<uint32_t>(FrameSyncProcess::audio_frame_length);
    if (arm_rfft_fast_init_f32(&rfft_instance_, fftSize) == ARM_MATH_SUCCESS) {
        initialized_ = true;
    }
}

auto IFFT::Execute(const FrameSyncProcess::AudioFrame &frame) -> FrameSyncProcess::AudioFrame {
    const auto fftSize = static_cast<uint32_t>(frame.size());
    if (fftSize < 2 || !initialized_) {
        return frame;
    }

    // arm_rfft_fast_f32 は入力バッファを書き換えるため, スタックではなくメンババッファにコピーする.
    input_buffer_ = frame;
    arm_rfft_fast_f32(&rfft_instance_, input_buffer_.data(), frame_buffer_.data(), 1);

    return frame_buffer_;
}
