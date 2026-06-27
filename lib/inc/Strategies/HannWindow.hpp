///
/// @file HannWindow.hpp
///
#pragma once

#include "FrameSyncProcess.hpp"

class HannWindow {
public:
    HannWindow();

    auto Execute(const FrameSyncProcess::AudioFrame &frame) -> FrameSyncProcess::AudioFrame;

private:
    FrameSyncProcess::AudioFrame window_;
    FrameSyncProcess::AudioFrame frame_buffer_;
};
