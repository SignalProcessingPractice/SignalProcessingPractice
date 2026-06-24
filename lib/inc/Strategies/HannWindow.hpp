///
/// @file HannWindow.hpp
///
#pragma once

#include "FrameSyncProcess.hpp"

class HannWindow {
public:
    HannWindow();

    FrameSyncProcess::AudioFrame Execute(FrameSyncProcess::AudioFrame &&frame);

private:
    FrameSyncProcess::AudioFrame window_{};
    FrameSyncProcess::AudioFrame frame_buffer_{};
};
