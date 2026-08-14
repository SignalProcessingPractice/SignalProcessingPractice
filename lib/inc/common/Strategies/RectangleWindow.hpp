///
/// @file RectangleWindow.hpp
///
#pragma once

#include "common/FrameSyncProcess.hpp"

class RectangleWindow {
public:
    RectangleWindow();

    static auto Exec(const FrameSyncProcess::AudioFrame& frame) -> FrameSyncProcess::AudioFrame;
    static auto Reset() -> void;
};
