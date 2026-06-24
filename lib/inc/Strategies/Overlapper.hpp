///
/// @file Overlapper.hpp
///
#pragma once

#include "FrameSyncProcess.hpp"

class Overlapper {
public:
    Overlapper() = default;

    FrameSyncProcess::AudioFrame Execute(FrameSyncProcess::AudioHop &&frame);

private:
    FrameSyncProcess::AudioFrame frame_buffer_{};
};
