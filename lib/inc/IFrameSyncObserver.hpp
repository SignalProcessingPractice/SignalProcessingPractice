///
/// @file IFrameSyncObserver.hpp
///
#pragma once

struct PipelineResult;

///
/// @brief FrameSyncProcess の Observer インターフェース.
///
/// FrameSyncProcess::ProcessFrame() の完了後に OnProcessFrame() が DSP スレッドで同期呼び出しされる.
/// 実装クラスはブロッキング処理を行ってはならない.
///
class IFrameSyncObserver {
public:
    IFrameSyncObserver() = default;
    IFrameSyncObserver(const IFrameSyncObserver&) = delete;
    auto operator=(const IFrameSyncObserver&) -> IFrameSyncObserver& = delete;
    IFrameSyncObserver(IFrameSyncObserver&&) = delete;
    auto operator=(IFrameSyncObserver&&) -> IFrameSyncObserver& = delete;
    virtual ~IFrameSyncObserver() = default;

    ///
    /// @brief 1 フレーム分の処理完了時に呼び出される.
    ///
    /// @param result 直前のフレームの信号処理結果.
    ///
    virtual auto OnProcessFrame(const PipelineResult& result) -> void = 0;
};
