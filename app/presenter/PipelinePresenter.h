///
/// @file PipelinePresenter.h
///
#pragma once

class QComboBox;

///
/// @brief 音声処理パイプライン各段の Strategy 選択 ComboBox 群.
///
/// View 側 (MainWindow) がウィジェットへのポインタを詰めて Presenter へ渡す.
///
struct PipelineComboBoxes {
    QComboBox* acquire{nullptr};
    QComboBox* pre_process{nullptr};
    QComboBox* overlap{nullptr};
    QComboBox* window{nullptr};
    QComboBox* fft{nullptr};
    QComboBox* infer{nullptr};
    QComboBox* post_process{nullptr};
    QComboBox* overlap_add{nullptr};
    QComboBox* output{nullptr};
};

///
/// @brief 音声処理パイプライン設定の Presenter 層.
///
/// TODO: MainModel を保持し, ComboBox の選択変更を
///       FrameSyncProcess::SetConfig() へ仲介する処理を実装する.
///
class PipelinePresenter {
public:
    explicit PipelinePresenter(const PipelineComboBoxes& combo_boxes);

private:
    ///
    /// 各段の Strategy 選択 ComboBox (View).
    ///
    PipelineComboBoxes combo_boxes_;
};
