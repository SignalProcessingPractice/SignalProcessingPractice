///
/// @file MainWindow.cpp
///

#include "MainWindow.h"

#include <QComboBox>
#include <QTimer>
#include <QVBoxLayout>
#include <array>
#include <memory>
#include <utility>

#include "common/AudioConfig.h"
#include "ui_MainWindow.h"
#include "view/PlotWidget.h"

namespace {

///
/// 表示更新周期 (約 30 fps).
///
constexpr int kFrameTickIntervalMs = 33;

///
/// 波形表示の振幅レンジ.
///
constexpr float kWaveformMin = -1.0F;
constexpr float kWaveformMax = 1.0F;

///
/// 波形表示の時間レンジ (1 ホップ分, ミリ秒).
///
constexpr float kMillisecondsPerSecond = 1000.0F;
constexpr float kHopDurationMs = kMillisecondsPerSecond *
                                 static_cast<float>(FrameSyncProcess::audio_hop_length) /
                                 static_cast<float>(kAppSampleRate);

///
/// スペクトラム表示の dB レンジ.
///
constexpr float kSpectrumMinDb = -100.0F;
constexpr float kSpectrumMaxDb = 0.0F;

///
/// スペクトラム表示の周波数レンジ (DC〜ナイキスト, kHz).
///
constexpr float kHzPerKHz = 1000.0F;
constexpr float kNyquistKHz = static_cast<float>(kAppSampleRate) / 2.0F / kHzPerKHz;

}  // namespace

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    SetupPipelineComboBoxes();
    SetupPlotWidgets();
    SetupFrameTick();
}

MainWindow::~MainWindow() {
    delete ui;
}

auto MainWindow::GetInferResultWidget() const -> QWidget* {
    return ui->widgetInferResult;
}

void MainWindow::AttachPipelineObserver(PipelineSelectionObserver observer) {
    pipeline_observers_.push_back(std::move(observer));
}

void MainWindow::AttachFrameTickObserver(FrameTickObserver observer) {
    frame_tick_observers_.push_back(std::move(observer));
}

void MainWindow::UpdateWaveform(std::span<const float> samples) {
    waveform_plot_->SetSamples(samples);
}

void MainWindow::UpdateSpectrum(std::span<const float> values) {
    spectrum_plot_->SetSamples(values);
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

namespace {

///
/// プレースホルダへ PlotWidget を埋め込む.
///
/// 生成したオブジェクトの所有権は Qt の親子機構 (placeholder) へ移譲する.
///
auto EmbedPlotWidget(QWidget* placeholder) -> PlotWidget* {
    auto plot = std::make_unique<PlotWidget>();
    auto layout = std::make_unique<QVBoxLayout>();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(plot.get());

    auto* plot_ptr = plot.release();
    placeholder->setLayout(layout.release());
    return plot_ptr;
}

}  // namespace

void MainWindow::SetupPlotWidgets() {
    waveform_plot_ = EmbedPlotWidget(ui->widgetWaveform);
    waveform_plot_->SetAxes(
            {.min_value = 0.0F, .max_value = kHopDurationMs, .unit = QStringLiteral("ms")},
            {.min_value = kWaveformMin, .max_value = kWaveformMax, .unit = QString{}});

    spectrum_plot_ = EmbedPlotWidget(ui->widgetSpectrum);
    spectrum_plot_->SetAxes(
            {.min_value = 0.0F, .max_value = kNyquistKHz, .unit = QStringLiteral("kHz")},
            {.min_value = kSpectrumMinDb,
             .max_value = kSpectrumMaxDb,
             .unit = QStringLiteral("dB")});
}

void MainWindow::SetupFrameTick() {
    // 所有権は Qt の親子機構 (this) へ移譲する.
    auto frame_tick_timer = std::make_unique<QTimer>(this);
    connect(frame_tick_timer.get(), &QTimer::timeout, this, [this] {
        NotifyFrameTick();
    });
    frame_tick_timer->start(kFrameTickIntervalMs);
    frame_tick_timer_ = frame_tick_timer.release();
}

void MainWindow::NotifyPipelineSelection(PipelineStage stage, int index) {
    for (const auto& observer : pipeline_observers_) {
        observer(stage, index);
    }
}

void MainWindow::NotifyFrameTick() {
    for (const auto& observer : frame_tick_observers_) {
        observer();
    }
}
