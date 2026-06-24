///
/// @file null_strategies.cpp
///
#include "Strategies/null_strategies.hpp"

#include "FrameSyncProcess.hpp"

FrameSyncProcess::AudioHop null_input(void) {
    // 空のフレームをムーブ (RVO) で返す.
    FrameSyncProcess::AudioHop frame;
    return frame;
}

FrameSyncProcess::AudioHop through_preprocess(FrameSyncProcess::AudioHop &&frame) {
    // 何もしないでフレームをそのまま返す.
    return frame;
}

FrameSyncProcess::AudioFrame through_postprocess(FrameSyncProcess::AudioFrame &&frame) {
    // 何もしないでフレームをそのまま返す.
    return frame;
}

FrameSyncProcess::AudioFrame null_window(FrameSyncProcess::AudioFrame &&frame) {
    // 何もしないでフレームをそのまま返す.
    return frame;
}

FrameSyncProcess::AudioFrame through_infer(FrameSyncProcess::AudioFrame &&frame) {
    ///
    /// 推論を行わず, フレームをそのまま返す.
    ///
    return frame;
}

FrameSyncProcess::AudioFrame null_infer(FrameSyncProcess::AudioFrame &&frame) {
    ///
    /// 推論を行わず, フレームを無音化して返す.
    ///
    std::fill(frame.begin(), frame.end(), 0.0f);
    return std::move(frame);
}

void null_output(FrameSyncProcess::AudioHop &&frame) {
    // 何もしないでフレームを破棄する.
}
