///
/// @file MainModel.cpp
///

#include "model/MainModel.h"

#include "FrameSyncProcessConfig.hpp"

MainModel::MainModel()
    : sine_generator_({.frequency = SineGenerator::kDefaultFrequency,
                       .amplitude = SineGenerator::kDefaultAmplitude}) {
}

void MainModel::ApplyStrategySelection(PipelineStage stage, int index) {
    switch (stage) {
        case PipelineStage::kAcquire:
            process_.SetConfig(FrameSyncProcess::AcquireTag{},
                               index == 1 ? FrameSyncProcess::AudioAcquireStrategy{&sine_generator_}
                                          : get_default_null_input_strategy());
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
