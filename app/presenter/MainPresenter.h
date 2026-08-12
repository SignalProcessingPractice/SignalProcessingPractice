///
/// @file MainPresenter.h
///
#pragma once

#include <memory>

#include "model/MainModel.h"
#include "presenter/InferResultPresenter.h"
#include "presenter/PipelinePresenter.h"
#include "presenter/SpectrumPresenter.h"
#include "presenter/WaveformPresenter.h"

class MainWindow;

///
/// @brief MVP の Presenter 層の統括.
///
/// MainWindow と紐づき, MainModel を所有し, 各表示エリア・パイプライン設定を
/// 担当する子 Presenter を保持する.
///
class MainPresenter {
public:
    explicit MainPresenter(MainWindow* view);

private:
    ///
    /// 紐づく View.
    ///
    MainWindow* view_;

    ///
    /// Model (子 Presenter と共有する).
    ///
    std::unique_ptr<MainModel> model_;

    ///
    /// @name 子 Presenter.
    /// {@
    WaveformPresenter waveform_presenter_;
    SpectrumPresenter spectrum_presenter_;
    InferResultPresenter infer_result_presenter_;
    PipelinePresenter pipeline_presenter_;
    /// @}
};
