///
/// @file RectangleWindow.cpp
///
#include "Strategies/RectangleWindow.hpp"

RectangleWindow::RectangleWindow() = default;

auto RectangleWindow::Execute(const FrameSyncProcess::AudioFrame &frame)
        -> FrameSyncProcess::AudioFrame {
    return frame;
}
