///
/// @file RingBufferAcquire.h
///
#pragma once

#include "FrameSyncProcess.hpp"

class AudioInputBuffer;

///
/// @brief リングバッファから 1 ホップ読み出す Acquire Strategy.
///
/// 入力種別 (無音 / Sine / 将来のデバイス) が変わっても,
/// FrameSyncProcess に bind するのは常にこの Strategy とする.
///
class RingBufferAcquire {
public:
    explicit RingBufferAcquire(AudioInputBuffer* buffer);

    auto Exec() -> FrameSyncProcess::AudioHop;
    auto Reset() -> void;

private:
    AudioInputBuffer* buffer_;
};
