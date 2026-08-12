///
/// @file RingBufferOutput.h
///
#pragma once

#include "FrameSyncProcess.hpp"

class AudioOutputBuffer;

///
/// @brief パイプラインの出力ホップをリングバッファへ書き込む Output Strategy.
///
/// 出力先 (デバイス等) が変わっても, FrameSyncProcess に bind するのは常にこの Strategy とする.
///
class RingBufferOutput {
public:
    explicit RingBufferOutput(AudioOutputBuffer* buffer);

    auto Exec(const FrameSyncProcess::AudioHop& hop) -> void;
    auto Reset() -> void;

private:
    AudioOutputBuffer* buffer_;
};
