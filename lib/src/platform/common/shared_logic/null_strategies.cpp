///
/// @file null_strategies.cpp
///
#include "Strategies/null_strategies.hpp"

#include "FrameSyncProcess.hpp"

FrameSyncProcess::AudioHop
    null_input (
        void
    )
{
    // 空のフレームをムーブ (RVO) で返す.
    FrameSyncProcess::AudioHop frame;
    return frame;
}

FrameSyncProcess::AudioHop
    null_preprocess (
        FrameSyncProcess::AudioHop &&frame
    )
{
    // 何もしないでフレームをそのまま返す.
    return frame;
}

FrameSyncProcess::AudioFrame
    null_postprocess (
        FrameSyncProcess::AudioFrame &&frame
    )
{
    // 何もしないでフレームをそのまま返す.
    return frame;
}

FrameSyncProcess::AudioFrame
    null_window (
        FrameSyncProcess::AudioFrame &&frame
    )
{
    // 何もしないでフレームをそのまま返す.
    return frame;
}

void
    null_output (
        FrameSyncProcess::AudioHop &&frame
    )
{
    // 何もしないでフレームを破棄する.
}
