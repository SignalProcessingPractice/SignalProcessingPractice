///
/// @file InferResultPresenter.h
///
#pragma once

class QWidget;

///
/// @brief 推論結果表示の Presenter 層.
///
/// TODO: MainModel を保持し, 推論結果データの取得と View への描画指示を実装する.
///
class InferResultPresenter {
public:
    explicit InferResultPresenter(QWidget* widget);

private:
    ///
    /// 描画対象のウィジェット (View).
    ///
    QWidget* widget_;
};
