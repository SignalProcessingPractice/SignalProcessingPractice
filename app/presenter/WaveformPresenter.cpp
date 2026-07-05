///
/// @file WaveformPresenter.cpp
///

#include "presenter/WaveformPresenter.h"

#include <utility>

#include "model/MainModel.h"

WaveformPresenter::WaveformPresenter(MainModel* model, const TickRegistrar& registrar,
                                     RenderFn render)
    : model_(model), render_(std::move(render)) {
    registrar([this] {
        OnTick();
    });
}

void WaveformPresenter::OnTick() {
    model_->Process().GetResult(&result_);
    render_(std::span<const float>{result_.input_hop.data(), result_.input_hop.size()});
}
