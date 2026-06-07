///
/// @file OverlapAdder.hpp
///
#pragma once

#include "FrameSyncProcess.hpp"

///
/// @brief 矩形波窓を使用した Overlap-Add.
///
/// @todo  Overlap-Add はあまり真面目に実装していないので, 必要に応じて見直す.
///
class OverlapAdder
{
public:
    OverlapAdder() = default;

    FrameSyncProcess::AudioHop
        Execute(
            FrameSyncProcess::AudioFrame &&frame
        );

private:

    FrameSyncProcess::AudioFrame frame_buffer_{};
};
