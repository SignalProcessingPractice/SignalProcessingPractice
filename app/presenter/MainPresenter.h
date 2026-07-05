///
/// @file MainPresenter.h
///
#pragma once

#include "presenter/InferResultPresenter.h"
#include "presenter/PipelinePresenter.h"
#include "presenter/SpectrumPresenter.h"
#include "presenter/WaveformPresenter.h"

class MainWindow;

///
/// @brief MVP の Presenter 層の統括.
///
/// MainWindow と紐づき, 各表示エリア・パイプライン設定を担当する子 Presenter を保持する.
///
/// TODO: MainModel の所有と各子 Presenter への共有を実装する.
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
    /// @name 子 Presenter.
    /// {@
    WaveformPresenter waveform_presenter_;
    SpectrumPresenter spectrum_presenter_;
    InferResultPresenter infer_result_presenter_;
    PipelinePresenter pipeline_presenter_;
    /// @}
};
