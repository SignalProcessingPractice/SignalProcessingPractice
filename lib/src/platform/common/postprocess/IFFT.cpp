///
/// @file IFFT.cpp
///
#include "Strategies/IFFT.hpp"

#include "arm_math.h"
#include <algorithm>

IFFT::IFFT()
{
    // コンストラクタで IFFT インスタンスを初期化する（固定サイズを想定）
    const uint32_t fftSize = static_cast<uint32_t>(FrameSyncProcess::audio_frame_length);
    if (arm_rfft_fast_init_f32(&rfft_instance_, fftSize) == ARM_MATH_SUCCESS)
    {
        initialized_ = true;
    }
}

FrameSyncProcess::AudioFrame
    IFFT::Execute(
        FrameSyncProcess::AudioFrame&& frame)
{
    const uint32_t fftSize = static_cast<uint32_t>(frame.size());
    if (fftSize < 2)
    {
        return frame;
    }
    if (!initialized_)
    {
        // 初期化に失敗している場合は入力をそのまま返す
        return frame;
    }

    // 出力は組み込み向けに事前確保したメンババッファに書き込む
    arm_rfft_fast_f32(&rfft_instance_, frame.data(), frame_buffer_.data(), 1);

    return frame_buffer_;
}
