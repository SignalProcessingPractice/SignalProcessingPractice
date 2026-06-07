///
/// @file OverlapAdder.hpp
///
#pragma once

#include "FrameSyncProcess.hpp"

class OverlapAdder
{
public:
    OverlapAdder() = default;

    FrameSyncProcess::AudioHop
        Execute(
            FrameSyncProcess::AudioFrame &&frame
        );

private:

    FrameSyncProcess::AudioFrame frame_buffer_{};
};
