///
/// @file FFT.cpp
///
#include "Strategies/FFT.hpp"

#include "arm_math.h"

FFT::FFT() {
    // コンストラクタで FFT インスタンスを初期化する（固定サイズを想定）
    const auto fftSize = static_cast<uint32_t>(FrameSyncProcess::audio_frame_length);
    if (arm_rfft_fast_init_f32(&rfft_instance_, fftSize) == ARM_MATH_SUCCESS) {
        initialized_ = true;
    }
}

auto FFT::Exec(const FrameSyncProcess::AudioFrame& frame) -> FrameSyncProcess::AudioFrame {
    const auto fftSize = static_cast<uint32_t>(frame.size());
    if (fftSize < 2 || !initialized_) {
        return frame;
    }

    // arm_rfft_fast_f32 は入力バッファを書き換えるため, スタックではなくメンババッファにコピーする.
    input_buffer_ = frame;
    arm_rfft_fast_f32(&rfft_instance_, input_buffer_.data(), frame_buffer_.data(), 0);

    return frame_buffer_;
}

auto FFT::Reset() -> void {
    input_buffer_ = FrameSyncProcess::AudioFrame{};
    frame_buffer_ = FrameSyncProcess::AudioFrame{};
}
