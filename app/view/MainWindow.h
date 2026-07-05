///
/// @file MainWindow.h
///
#pragma once

#include <QMainWindow>
#include <vector>

#include "common/PipelineSelection.h"

namespace Ui {
class MainWindow;
}

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
    [[nodiscard]] auto GetWaveformWidget() const -> QWidget*;
    [[nodiscard]] auto GetSpectrumWidget() const -> QWidget*;
    [[nodiscard]] auto GetInferResultWidget() const -> QWidget*;
    /// @}

    ///
    /// @brief パイプライン Strategy 選択変更の Observer を登録する.
    ///
    /// 登録した Observer は, 音声処理パイプラインの ComboBox 変更時に
    /// (段, 選択 index) で呼び出される.
    ///
    void AttachPipelineObserver(PipelineSelectionObserver observer);

private:
    ///
    /// 各 ComboBox への選択肢投入とシグナル接続.
    ///
    void SetupPipelineComboBoxes();

    ///
    /// 登録済み Observer への通知.
    ///
    void NotifyPipelineSelection(PipelineStage stage, int index);

    Ui::MainWindow* ui;

    ///
    /// パイプライン Strategy 選択変更の Observer リスト.
    ///
    std::vector<PipelineSelectionObserver> pipeline_observers_;
};
