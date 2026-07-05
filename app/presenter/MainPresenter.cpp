///
/// @file MainPresenter.cpp
///

#include "presenter/MainPresenter.h"

#include <functional>
#include <utility>

#include "view/MainWindow.h"

MainPresenter::MainPresenter(MainWindow* view)
    : view_(view),
      model_(std::make_unique<MainModel>()),
      waveform_presenter_(
              model_.get(),
              [view](std::function<void()> observer) {
                  view->AttachFrameTickObserver(std::move(observer));
              },
              [view](std::span<const float> samples) {
                  view->UpdateWaveform(samples);
              }),
      spectrum_presenter_(
              model_.get(),
              [view](std::function<void()> observer) {
                  view->AttachFrameTickObserver(std::move(observer));
              },
              [view](std::span<const float> values) {
                  view->UpdateSpectrum(values);
              }),
      infer_result_presenter_(view->GetInferResultWidget()),
      pipeline_presenter_(model_.get(), [view](PipelineSelectionObserver observer) {
          view->AttachPipelineObserver(std::move(observer));
      }) {
    // NOTE: 開始/停止 UI の実装までは, Presenter 構築と同時に処理を開始する.
    model_->Start();
}
