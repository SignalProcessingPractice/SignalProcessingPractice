///
/// @file RectangleWindow.cpp
///
#include "Strategies/RectangleWindow.hpp"

RectangleWindow::RectangleWindow() = default;

auto RectangleWindow::Exec(const FrameSyncProcess::AudioFrame& frame)
        -> FrameSyncProcess::AudioFrame {
    return frame;
}

auto RectangleWindow::Reset() -> void {
}
