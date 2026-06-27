///
/// @file Overlapper.hpp
///
#pragma once

#include "FrameSyncProcess.hpp"

class Overlapper {
public:
    Overlapper() = default;

    auto Execute(const FrameSyncProcess::AudioHop &frame) -> FrameSyncProcess::AudioFrame;

private:
    FrameSyncProcess::AudioFrame frame_buffer_;
};
