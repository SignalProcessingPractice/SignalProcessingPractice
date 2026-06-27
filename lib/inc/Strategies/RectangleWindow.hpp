///
/// @file RectangleWindow.hpp
///
#pragma once

#include "FrameSyncProcess.hpp"

class RectangleWindow {
public:
    RectangleWindow();

    auto Execute(const FrameSyncProcess::AudioFrame &frame) -> FrameSyncProcess::AudioFrame;
};
