///
/// @file MainModel.cpp
///

#include "model/MainModel.h"

#include "FrameSyncProcessConfig.hpp"
#include "PipelineResult.hpp"
#include "common/AudioConfig.h"
#include "model/DeviceInput.h"

MainModel::MainModel()
    : device_input_(std::make_unique<DeviceInput>(&input_buffer_)),
      sine_generator_({.frequency = SineGenerator::kDefaultFrequency,
                       .amplitude = SineGenerator::kDefaultAmplitude}) {
    process_.SetConfig(FrameSyncProcess::AcquireTag{},
                       FrameSyncProcess::AudioAcquireStrategy{&ring_buffer_acquire_});
    process_.Attach(
            FrameSyncProcess::ObserverDelegate::create<MainModel, &MainModel::OnFrameProcessed>(
                    *this));
}

MainModel::~MainModel() {
    Stop();
}

void MainModel::Start() {
    if (processing_thread_.joinable()) {
        return;
    }
    input_source_.Start();
    processing_thread_ = std::jthread{[this](const std::stop_token& stop_token) {
        RunProcessing(stop_token);
    }};
}

void MainModel::Stop() {
    device_input_->Stop();
    input_source_.Stop();
    if (processing_thread_.joinable()) {
        processing_thread_.request_stop();
        processing_thread_.join();
    }
}

void MainModel::ApplyStrategySelection(PipelineStage stage, int index) {
    switch (stage) {
        case PipelineStage::kAcquire:
            // 入力は Acquire Strategy を差し替えず, Producer を排他的に切り替える.
            if (index == kAcquireDeviceItemIndex) {
                input_source_.Stop();
                device_input_->Stop();
                // キャプチャの開始はデバイス選択 (ApplyDeviceSelection) を待つ.
                device_mode_ = true;
            } else {
                device_mode_ = false;
                device_input_->Stop();
                if (index == kAcquireSineItemIndex) {
                    input_source_.SetGenerator([this] {
                        return sine_generator_.Exec();
                    });
                } else {
                    input_source_.SetGenerator([] {
                        return FrameSyncProcess::AudioHop{kAppSampleRate};
                    });
                }
                input_source_.Start();
            }
            break;
        case PipelineStage::kPreProcess:
            process_.SetConfig(FrameSyncProcess::PreProcessTag{},
                               get_default_through_preprocess_strategy());
            break;
        case PipelineStage::kOverlap:
            process_.SetConfig(FrameSyncProcess::OverlapTag{}, get_default_overlapper_strategy());
            break;
        case PipelineStage::kWindow:
            process_.SetConfig(FrameSyncProcess::WindowTag{},
                               index == 1 ? FrameSyncProcess::WindowStrategy{&hann_window_}
                                          : get_default_rectangle_window_strategy());
            break;
        case PipelineStage::kFft:
            process_.SetConfig(FrameSyncProcess::FftTag{}, get_default_fft_strategy());
            break;
        case PipelineStage::kInfer:
            process_.SetConfig(FrameSyncProcess::InferTag{}, get_default_through_infer_strategy());
            break;
        case PipelineStage::kPostProcess:
            process_.SetConfig(FrameSyncProcess::PostProcessTag{},
                               get_default_ifft_postprocess_strategy());
            break;
        case PipelineStage::kOverlapAdd:
            process_.SetConfig(FrameSyncProcess::OverlapAddTag{},
                               index == 1
                                       ? FrameSyncProcess::OverlapAddStrategy{&hann_overlap_adder_}
                                       : get_default_rectangle_overlap_adder_strategy());
            break;
        case PipelineStage::kOutput:
            process_.SetConfig(FrameSyncProcess::OutputTag{}, get_default_null_output_strategy());
            break;
    }
}

auto MainModel::GetAudioInputDeviceNames() -> std::vector<std::string> {
    return DeviceInput::GetDeviceNames();
}

void MainModel::ApplyDeviceSelection(int device_index) {
    if (!device_mode_) {
        return;
    }
    device_input_->Stop();
    device_input_->Start(device_index);
}

void MainModel::ApplySineFrequency(int frequency_hz) {
    // Producer スレッドの Exec() と排他して周波数を更新する.
    input_source_.RunWithGeneratorLock([this, frequency_hz] {
        sine_generator_.SetFrequency(static_cast<double>(frequency_hz));
    });
}

auto MainModel::Process() -> FrameSyncProcess& {
    return process_;
}

auto MainModel::GetProcessedFrameCount() const -> std::uint64_t {
    return processed_frame_count_.load(std::memory_order_relaxed);
}

void MainModel::RunProcessing(const std::stop_token& stop_token) {
    while (input_buffer_.WaitForHop(stop_token)) {
        process_.ProcessFrame();
    }
}

void MainModel::OnFrameProcessed([[maybe_unused]] const PipelineResult& result) {
    processed_frame_count_.fetch_add(1, std::memory_order_relaxed);
}
