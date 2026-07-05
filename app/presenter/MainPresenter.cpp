///
/// @file MainPresenter.cpp
///

#include "presenter/MainPresenter.h"

#include <utility>

#include "view/MainWindow.h"

MainPresenter::MainPresenter(MainWindow* view)
    : view_(view),
      model_(std::make_unique<MainModel>()),
      waveform_presenter_(view->GetWaveformWidget()),
      spectrum_presenter_(view->GetSpectrumWidget()),
      infer_result_presenter_(view->GetInferResultWidget()),
      pipeline_presenter_(model_.get(), [view](PipelineSelectionObserver observer) {
          view->AttachPipelineObserver(std::move(observer));
      }) {
}
