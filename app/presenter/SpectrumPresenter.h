///
/// @file SpectrumPresenter.h
///
#pragma once

class QWidget;

///
/// @brief 音声波形（周波数軸）表示の Presenter 層.
///
/// TODO: MainModel を保持し, 周波数軸データの取得と View への描画指示を実装する.
///
class SpectrumPresenter {
public:
    explicit SpectrumPresenter(QWidget* widget);

private:
    ///
    /// 描画対象のウィジェット (View).
    ///
    QWidget* widget_;
};
