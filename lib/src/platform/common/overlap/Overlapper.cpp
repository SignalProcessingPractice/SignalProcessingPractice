///
/// @file Overlapper.cpp
///
#include "Strategies/Overlapper.hpp"

#include "FrameSyncProcess.hpp"

///
/// @brief オーバーラッピング処理.
///
/// @note  Pipeline 上の各 Strategy と抽象化を合わせておく意図で, 
///        オーバーラッピング処理の具象ストラテジを本クラスに別けて
///        実装している. ただし, オーバーラッピング処理に他の具象
///        ストラテジを実装する状況はまず無いだろう.
///
FrameSyncProcess::AudioFrame
    Overlapper::Execute(
        FrameSyncProcess::AudioHop&& hop
    )
{
    constexpr auto hop_size =
        FrameSyncProcess::audio_hop_length;

    std::move(
        frame_buffer_.begin() + hop_size,
        frame_buffer_.end(),
        frame_buffer_.begin());

    std::copy(
        hop.begin(),
        hop.end(),
        frame_buffer_.end() - hop_size);

    return frame_buffer_;
}
