///
/// @file do_nothing.cpp
///
#include "FrameSyncProcess.hpp"

FrameSyncProcess::AudioFrame
    do_nothing (
        FrameSyncProcess::AudioFrame &&frame
    )
{
    // 何もしないでフレームをそのまま返す.
    return frame;
}
