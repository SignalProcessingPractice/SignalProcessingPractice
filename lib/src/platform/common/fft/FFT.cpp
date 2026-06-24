///
/// @file FFT.cpp
///
#include "Strategies/FFT.hpp"

#include <algorithm>

#include "arm_math.h"

FFT::FFT() {
    // コンストラクタで FFT インスタンスを初期化する（固定サイズを想定）
    const uint32_t fftSize = static_cast<uint32_t>(FrameSyncProcess::audio_frame_length);
    if (arm_rfft_fast_init_f32(&rfft_instance_, fftSize) == ARM_MATH_SUCCESS) {
        initialized_ = true;
    }
}

FrameSyncProcess::AudioFrame FFT::Execute(FrameSyncProcess::AudioFrame&& frame) {
    const uint32_t fftSize = static_cast<uint32_t>(frame.size());
    if (fftSize < 2) {
        return frame;
    }
    if (!initialized_) {
        // 初期化に失敗している場合は入力をそのまま返す
        return frame;
    }

    // 出力は組み込み向けに事前確保したメンババッファに書き込む
    arm_rfft_fast_f32(&rfft_instance_, frame.data(), frame_buffer_.data(), 0);

    return frame_buffer_;
}
