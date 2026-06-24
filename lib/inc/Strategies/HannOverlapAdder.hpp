///
/// @file HannOverlapAdder.hpp
///
#pragma once

#include "FrameSyncProcess.hpp"

///
/// @brief ハニング窓を使用した Overlap-Add.
///
class HannOverlapAdder {
public:
    HannOverlapAdder();

    FrameSyncProcess::AudioHop Execute(FrameSyncProcess::AudioFrame &&frame);

private:
    FrameSyncProcess::AudioHop hop_buffer_{};
    FrameSyncProcess::AudioFrame window_{};
    FrameSyncProcess::AudioFrame windowed_buffer_{};
    FrameSyncProcess::AudioFrame frame_buffer_{};
};
