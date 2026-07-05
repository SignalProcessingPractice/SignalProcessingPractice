///
/// @file MainModel.h
///
#pragma once

#include "FrameSyncProcess.hpp"
#include "Strategies/HannOverlapAdder.hpp"
#include "Strategies/HannWindow.hpp"
#include "Strategies/SineGenerator.hpp"
#include "common/PipelineSelection.h"

///
/// @brief MVP の Model 層.
///
/// FrameSyncProcess を所有し, Strategy の選択状態を管理する.
///
/// TODO: 処理スレッドの管理 (ProcessFrame() の駆動) を実装する.
///
class MainModel {
public:
    MainModel();

    ///
    /// ComboBox の選択 (stage, index) に対応する Strategy を FrameSyncProcess へ設定する.
    ///
    /// index は GetStrategyNames(stage) の並びに対応する.
    ///
    void ApplyStrategySelection(PipelineStage stage, int index);

    ///
    /// FrameSyncProcess への参照を取得する.
    ///
    [[nodiscard]] auto Process() -> FrameSyncProcess&;

private:
    FrameSyncProcess process_;

    ///
    /// @name 代替 Strategy の実体.
    ///
    /// StrategySlot は非所有ポインタを bind するため, 実体は MainModel が所有する.
    /// 既定 (index 0) の Strategy は FrameSyncProcessConfig の static 実体を再利用する.
    /// {@
    SineGenerator sine_generator_;
    HannWindow hann_window_;
    HannOverlapAdder hann_overlap_adder_;
    /// @}
};
