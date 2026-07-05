///
/// @file PipelinePresenter.h
///
#pragma once

#include <functional>

#include "common/PipelineSelection.h"

class MainModel;

///
/// @brief 音声処理パイプライン設定の Presenter 層.
///
/// 初期化時に View の Observer 機構へ Observer を登録し,
/// ComboBox の選択変更を MainModel の FrameSyncProcess::SetConfig() へ仲介する.
///
class PipelinePresenter {
public:
    ///
    /// Observer 登録関数の型.
    ///
    /// View (MainWindow) の AttachPipelineObserver() を注入することで,
    /// Presenter が View の具象型へ依存しない形で Observer を登録する.
    ///
    using ObserverRegistrar = std::function<void(PipelineSelectionObserver)>;

    PipelinePresenter(MainModel* model, const ObserverRegistrar& registrar);

private:
    ///
    /// ComboBox 選択変更時のイベントハンドラ.
    ///
    void OnStrategySelected(PipelineStage stage, int index);

    ///
    /// 紐づく Model.
    ///
    MainModel* model_;
};
