///
/// @file WaveformPresenter.h
///
#pragma once

class QWidget;

///
/// @brief 音声波形（時間軸）表示の Presenter 層.
///
/// TODO: MainModel を保持し, 時間軸波形データの取得と View への描画指示を実装する.
///
class WaveformPresenter {
public:
    explicit WaveformPresenter(QWidget* widget);

private:
    ///
    /// 描画対象のウィジェット (View).
    ///
    QWidget* widget_;
};
