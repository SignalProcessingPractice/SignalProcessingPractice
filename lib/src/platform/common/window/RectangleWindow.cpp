///
/// @file RectangleWindow.cpp
///
#include "Strategies/RectangleWindow.hpp"

RectangleWindow::RectangleWindow() {
}

FrameSyncProcess::AudioFrame RectangleWindow::Execute(FrameSyncProcess::AudioFrame&& frame) {
    return frame;
}
