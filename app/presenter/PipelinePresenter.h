///
/// @file PipelinePresenter.h
///
#pragma once

#include <functional>
#include <string>
#include <vector>

#include "common/PipelineSelection.h"

class MainModel;

///
/// @brief View (MainWindow) から注入されるフック群.
///
/// Presenter が View の具象型へ依存しないための Type Erasure 境界.
///
struct PipelineViewHooks {
    ///
    /// Strategy 選択変更の Observer 登録関数 (AttachPipelineObserver() を注入する).
    ///
    std::function<void(PipelineSelectionObserver)> attach_selection;

    ///
    /// 入力デバイス選択変更の Observer 登録関数 (AttachAcquireDeviceObserver() を注入する).
    ///
    std::function<void(AcquireDeviceObserver)> attach_device;

    ///
    /// デバイス選択 ComboBox の表示・選択肢投入 (ShowAcquireDeviceSelector() を注入する).
    ///
    std::function<void(const std::vector<std::string>&)> show_device_selector;

    ///
    /// デバイス選択 ComboBox の非表示化 (HideAcquireDeviceSelector() を注入する).
    ///
    std::function<void()> hide_device_selector;

    ///
    /// サイン波周波数変更の Observer 登録関数 (AttachSineFrequencyObserver() を注入する).
    ///
    std::function<void(SineFrequencyObserver)> attach_sine_frequency;

    ///
    /// 周波数 SpinBox の表示 (ShowSineFrequencySelector() を注入する).
    ///
    std::function<void()> show_sine_frequency_selector;

    ///
    /// 周波数 SpinBox の非表示化 (HideSineFrequencySelector() を注入する).
    ///
    std::function<void()> hide_sine_frequency_selector;
};

///
/// @brief 音声処理パイプライン設定の Presenter 層.
///
/// 初期化時に View の Observer 機構へ Observer を登録し,
/// ComboBox の選択変更を MainModel の FrameSyncProcess::SetConfig() へ仲介する.
/// 入力 "Device" 選択時はデバイス一覧を View へ渡し, デバイス選択を Model へ仲介する.
///
class PipelinePresenter {
public:
    PipelinePresenter(MainModel* model, PipelineViewHooks hooks);

private:
    ///
    /// ComboBox 選択変更時のイベントハンドラ.
    ///
    void OnStrategySelected(PipelineStage stage, int index);

    ///
    /// 入力デバイス選択変更時のイベントハンドラ.
    ///
    void OnDeviceSelected(int device_index);

    ///
    /// サイン波周波数変更時のイベントハンドラ.
    ///
    void OnSineFrequencyChanged(int frequency_hz);

    ///
    /// 紐づく Model.
    ///
    MainModel* model_;

    ///
    /// View から注入されたフック群.
    ///
    PipelineViewHooks hooks_;
};
