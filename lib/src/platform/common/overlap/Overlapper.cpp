///
/// @file Overlapper.cpp
///
#include "common/Strategies/Overlapper.hpp"

#include <iterator>

#include "common/FrameSyncProcess.hpp"

///
/// @brief オーバーラッピング処理.
///
/// @note  Pipeline 上の各 Strategy と抽象化を合わせておく意図で,
///        オーバーラッピング処理の具象ストラテジを本クラスに別けて
///        実装している. ただし, オーバーラッピング処理に他の具象
///        ストラテジを実装する状況はまず無いだろう.
///
auto Overlapper::Exec(const FrameSyncProcess::AudioHop& frame) -> FrameSyncProcess::AudioFrame
{
    constexpr auto hop_size = static_cast<std::ptrdiff_t>(FrameSyncProcess::audio_hop_length);

    std::move(std::next(frame_buffer_.begin(), hop_size), frame_buffer_.end(),
              frame_buffer_.begin());

    std::copy(frame.begin(), frame.end(), std::prev(frame_buffer_.end(), hop_size));

    return frame_buffer_;
}

auto Overlapper::Reset() -> void
{
    frame_buffer_ = FrameSyncProcess::AudioFrame{};
}
