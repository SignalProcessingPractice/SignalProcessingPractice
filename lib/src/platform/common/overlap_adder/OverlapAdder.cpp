///
/// @file OverlapAdder.cpp
///
#include "Strategies/OverlapAdder.hpp"

#include "FrameSyncProcess.hpp"

FrameSyncProcess::AudioHop
    OverlapAdder::Execute(
        FrameSyncProcess::AudioFrame&& frame
    )
{
    ///
    /// TODO: ここでは, 仮実装として, 空のフレームを返すだけの実装とする.
    ///       最終的には, ハニング窓を合成窓として使用した Overlap-Add を実装する予定.
    ///

    FrameSyncProcess::AudioHop hop;
    return hop;
}
