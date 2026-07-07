///
/// @file PipelinePresenter.cpp
///

#include "presenter/PipelinePresenter.h"

#include <utility>

#include "model/MainModel.h"

PipelinePresenter::PipelinePresenter(MainModel* model, PipelineViewHooks hooks)
    : model_(model), hooks_(std::move(hooks)) {
    hooks_.attach_selection([this](PipelineStage stage, int index) {
        OnStrategySelected(stage, index);
    });
    hooks_.attach_device([this](int device_index) {
        OnDeviceSelected(device_index);
    });
    hooks_.attach_sine_frequency([this](int frequency_hz) {
        OnSineFrequencyChanged(frequency_hz);
    });
    hooks_.attach_file_selection([this](const std::string& path) {
        OnFileSelected(path);
    });
    hooks_.attach_output_device([this](int device_index) {
        OnOutputDeviceSelected(device_index);
    });
}

void PipelinePresenter::OnStrategySelected(PipelineStage stage, int index) {
    model_->ApplyStrategySelection(stage, index);

    if (stage == PipelineStage::kOutput) {
        if (index == kOutputDeviceItemIndex) {
            hooks_.show_output_device_selector(MainModel::GetAudioOutputDeviceNames());
        } else {
            hooks_.hide_output_device_selector();
        }
    }

    if (stage == PipelineStage::kAcquire) {
        if (index == kAcquireDeviceItemIndex) {
            hooks_.hide_sine_frequency_selector();
            hooks_.hide_file_selector();
            hooks_.show_device_selector(MainModel::GetAudioInputDeviceNames());
        } else if (index == kAcquireSineItemIndex) {
            hooks_.hide_device_selector();
            hooks_.hide_file_selector();
            hooks_.show_sine_frequency_selector();
        } else if (index == kAcquireFileItemIndex) {
            hooks_.hide_device_selector();
            hooks_.hide_sine_frequency_selector();
            hooks_.show_file_selector();
        } else {
            hooks_.hide_device_selector();
            hooks_.hide_sine_frequency_selector();
            hooks_.hide_file_selector();
        }
    }
}

void PipelinePresenter::OnDeviceSelected(int device_index) {
    model_->ApplyDeviceSelection(device_index);
}

void PipelinePresenter::OnSineFrequencyChanged(int frequency_hz) {
    model_->ApplySineFrequency(frequency_hz);
}

void PipelinePresenter::OnFileSelected(const std::string& path) {
    model_->ApplyFileSelection(path);
}

void PipelinePresenter::OnOutputDeviceSelected(int device_index) {
    model_->ApplyOutputDeviceSelection(device_index);
}
