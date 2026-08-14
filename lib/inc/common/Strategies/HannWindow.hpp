///
/// @file HannWindow.hpp
///
#pragma once

#include "common/FrameSyncProcess.hpp"

class HannWindow {
public:
    HannWindow();

    auto Exec(const FrameSyncProcess::AudioFrame& frame) -> FrameSyncProcess::AudioFrame;
    auto Reset() -> void;

private:
    FrameSyncProcess::AudioFrame window_;
    FrameSyncProcess::AudioFrame frame_buffer_;
};
