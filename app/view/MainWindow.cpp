///
/// @file MainWindow.cpp
///

#include "MainWindow.h"

#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete ui;
}

auto MainWindow::GetWaveformWidget() const -> QWidget* {
    return ui->widgetWaveform;
}

auto MainWindow::GetSpectrumWidget() const -> QWidget* {
    return ui->widgetSpectrum;
}

auto MainWindow::GetInferResultWidget() const -> QWidget* {
    return ui->widgetInferResult;
}

auto MainWindow::GetPipelineComboBoxes() const -> PipelineComboBoxes {
    return PipelineComboBoxes{
            .acquire = ui->comboBoxAcquire,
            .pre_process = ui->comboBoxPreProcess,
            .overlap = ui->comboBoxOverlap,
            .window = ui->comboBoxWindow,
            .fft = ui->comboBoxFft,
            .infer = ui->comboBoxInfer,
            .post_process = ui->comboBoxPostProcess,
            .overlap_add = ui->comboBoxOverlapAdd,
            .output = ui->comboBoxOutput,
    };
}
