///
/// @file RectangleWindow.hpp
///
#pragma once

#include "FrameSyncProcess.hpp"

class RectangleWindow
{
public:
    RectangleWindow();

    FrameSyncProcess::AudioFrame
        Execute(
            FrameSyncProcess::AudioFrame &&frame
        );

};
