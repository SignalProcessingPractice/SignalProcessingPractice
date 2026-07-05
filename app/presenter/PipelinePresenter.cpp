///
/// @file PipelinePresenter.cpp
///

#include "presenter/PipelinePresenter.h"

#include "model/MainModel.h"

PipelinePresenter::PipelinePresenter(MainModel* model, const ObserverRegistrar& registrar)
    : model_(model) {
    registrar([this](PipelineStage stage, int index) {
        OnStrategySelected(stage, index);
    });
}

void PipelinePresenter::OnStrategySelected(PipelineStage stage, int index) {
    model_->ApplyStrategySelection(stage, index);
}
