///
/// @file Overlapper.hpp
///
#pragma once

#include "common/FrameSyncProcess.hpp"

class Overlapper {
public:
    Overlapper() = default;

    auto Exec(const FrameSyncProcess::AudioHop& frame) -> FrameSyncProcess::AudioFrame;
    auto Reset() -> void;

private:
    FrameSyncProcess::AudioFrame frame_buffer_;
};
