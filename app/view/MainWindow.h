///
/// @file MainWindow.h
///
#pragma once

#include <QMainWindow>
#include <functional>
#include <span>
#include <vector>

#include "common/PipelineSelection.h"

namespace Ui {
class MainWindow;
}

class PlotWidget;
class QTimer;

///
/// @brief 表示更新周期 (約 30 fps) の Tick Observer 型.
///
using FrameTickObserver = std::function<void()>;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    MainWindow(const MainWindow&) = delete;
    auto operator=(const MainWindow&) -> MainWindow& = delete;
    MainWindow(MainWindow&&) = delete;
    auto operator=(MainWindow&&) -> MainWindow& = delete;

    ///
    /// @name Presenter へ渡すウィジェットのアクセサ.
    /// {@
    [[nodiscard]] auto GetInferResultWidget() const -> QWidget*;
    /// @}

    ///
    /// @brief パイプライン Strategy 選択変更の Observer を登録する.
    ///
    /// 登録した Observer は, 音声処理パイプラインの ComboBox 変更時に
    /// (段, 選択 index) で呼び出される.
    ///
    void AttachPipelineObserver(PipelineSelectionObserver observer);

    ///
    /// @brief 表示更新周期の Tick Observer を登録する.
    ///
    /// 登録した Observer は QTimer により約 30 fps で呼び出される.
    ///
    void AttachFrameTickObserver(FrameTickObserver observer);

    ///
    /// @name Presenter からの描画指示.
    /// {@
    ///
    /// 時間軸波形を描画する (振幅レンジ ±1.0).
    ///
    void UpdateWaveform(std::span<const float> samples);

    ///
    /// 振幅スペクトラムを描画する (レンジ -100〜0 dB).
    ///
    void UpdateSpectrum(std::span<const float> values);
    /// @}

private:
    ///
    /// 各 ComboBox への選択肢投入とシグナル接続.
    ///
    void SetupPipelineComboBoxes();

    ///
    /// プレースホルダへの PlotWidget の埋め込み.
    ///
    void SetupPlotWidgets();

    ///
    /// 表示更新タイマーの起動.
    ///
    void SetupFrameTick();

    ///
    /// 登録済み Observer への通知.
    ///
    void NotifyPipelineSelection(PipelineStage stage, int index);
    void NotifyFrameTick();

    Ui::MainWindow* ui;

    PlotWidget* waveform_plot_{nullptr};
    PlotWidget* spectrum_plot_{nullptr};
    QTimer* frame_tick_timer_{nullptr};

    ///
    /// @name Observer リスト.
    /// {@
    std::vector<PipelineSelectionObserver> pipeline_observers_;
    std::vector<FrameTickObserver> frame_tick_observers_;
    /// @}
};
