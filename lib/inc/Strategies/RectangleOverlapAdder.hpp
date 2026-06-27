///
/// @file RectangleOverlapAdder.hpp
///
#pragma once

#include "FrameSyncProcess.hpp"

///
/// @brief 矩形波窓を使用した Overlap-Add.
///
class RectangleOverlapAdder {
public:
    RectangleOverlapAdder() = default;

    auto Execute(const FrameSyncProcess::AudioFrame &frame) -> FrameSyncProcess::AudioHop;

private:
    FrameSyncProcess::AudioHop hop_buffer_;
    FrameSyncProcess::AudioFrame frame_buffer_;
};
