///
/// @file MainWindow.h
///
#pragma once

#include <QMainWindow>

#include "presenter/PipelinePresenter.h"

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
    [[nodiscard]] auto GetPipelineComboBoxes() const -> PipelineComboBoxes;
    /// @}

private:
    Ui::MainWindow* ui;
};
