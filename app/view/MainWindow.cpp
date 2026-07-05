///
/// @file MainWindow.cpp
///

#include "MainWindow.h"

#include <QComboBox>
#include <array>
#include <utility>

#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    SetupPipelineComboBoxes();
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

void MainWindow::AttachPipelineObserver(PipelineSelectionObserver observer) {
    pipeline_observers_.push_back(std::move(observer));
}

void MainWindow::SetupPipelineComboBoxes() {
    const std::array<std::pair<PipelineStage, QComboBox*>, kPipelineStageCount> combo_boxes{{
            {PipelineStage::kAcquire, ui->comboBoxAcquire},
            {PipelineStage::kPreProcess, ui->comboBoxPreProcess},
            {PipelineStage::kOverlap, ui->comboBoxOverlap},
            {PipelineStage::kWindow, ui->comboBoxWindow},
            {PipelineStage::kFft, ui->comboBoxFft},
            {PipelineStage::kInfer, ui->comboBoxInfer},
            {PipelineStage::kPostProcess, ui->comboBoxPostProcess},
            {PipelineStage::kOverlapAdd, ui->comboBoxOverlapAdd},
            {PipelineStage::kOutput, ui->comboBoxOutput},
    }};

    for (const auto& [stage, combo_box] : combo_boxes) {
        for (const auto name : GetStrategyNames(stage)) {
            combo_box->addItem(QString::fromUtf8(name.data(), static_cast<qsizetype>(name.size())));
        }
        connect(combo_box, &QComboBox::currentIndexChanged, this, [this, stage](int index) {
            NotifyPipelineSelection(stage, index);
        });
    }
}

void MainWindow::NotifyPipelineSelection(PipelineStage stage, int index) {
    for (const auto& observer : pipeline_observers_) {
        observer(stage, index);
    }
}
