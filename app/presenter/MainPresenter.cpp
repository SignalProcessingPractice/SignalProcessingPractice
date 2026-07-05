///
/// @file MainPresenter.cpp
///

#include "presenter/MainPresenter.h"

#include "view/MainWindow.h"

MainPresenter::MainPresenter(MainWindow* view)
    : view_(view),
      waveform_presenter_(view->GetWaveformWidget()),
      spectrum_presenter_(view->GetSpectrumWidget()),
      infer_result_presenter_(view->GetInferResultWidget()),
      pipeline_presenter_(view->GetPipelineComboBoxes()) {
}
