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
}

void PipelinePresenter::OnStrategySelected(PipelineStage stage, int index) {
    model_->ApplyStrategySelection(stage, index);

    if (stage == PipelineStage::kAcquire) {
        if (index == kAcquireDeviceItemIndex) {
            hooks_.show_device_selector(MainModel::GetAudioInputDeviceNames());
        } else {
            hooks_.hide_device_selector();
        }
    }
}

void PipelinePresenter::OnDeviceSelected(int device_index) {
    model_->ApplyDeviceSelection(device_index);
}
