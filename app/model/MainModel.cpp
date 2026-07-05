///
/// @file MainModel.cpp
///

#include "model/MainModel.h"

#include "FrameSyncProcessConfig.hpp"
#include "PipelineResult.hpp"
#include "common/AudioConfig.h"

MainModel::MainModel()
    : sine_generator_({.frequency = SineGenerator::kDefaultFrequency,
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
    input_source_.Stop();
    if (processing_thread_.joinable()) {
        processing_thread_.request_stop();
        processing_thread_.join();
    }
}

void MainModel::ApplyStrategySelection(PipelineStage stage, int index) {
    switch (stage) {
        case PipelineStage::kAcquire:
            // 入力は Acquire Strategy を差し替えず, Producer の generator を切り替える.
            if (index == 1) {
                input_source_.SetGenerator([this] {
                    return sine_generator_.Exec();
                });
            } else {
                input_source_.SetGenerator([] {
                    return FrameSyncProcess::AudioHop{kAppSampleRate};
                });
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
