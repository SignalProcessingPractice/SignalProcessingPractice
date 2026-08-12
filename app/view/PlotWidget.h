///
/// @file PlotWidget.h
///
#pragma once

#include <span>
#include <vector>

#include <QString>
#include <QWidget>

///
/// 目盛り本数の既定値.
///
inline constexpr int kDefaultPlotTickCount = 5;

///
/// @brief プロットの軸設定.
///
struct PlotAxis {
    float min_value{0.0F};
    float max_value{1.0F};

    ///
    /// 単位表記 (最大値側の目盛りラベルに付記する). 空なら数値のみ.
    ///
    QString unit;

    ///
    /// 目盛りの本数 (2 以上).
    ///
    int tick_count{kDefaultPlotTickCount};
};

///
/// @brief 折れ線プロットの汎用描画ウィジェット.
///
/// 波形 (時間軸) とスペクトラム (周波数軸) の描画で共用する.
/// 縦軸・横軸の目盛り数値とグリッドを描画する.
///
class PlotWidget : public QWidget {
    Q_OBJECT

public:
    explicit PlotWidget(QWidget* parent = nullptr);

    ///
    /// 軸設定 (レンジ・単位・目盛り数) を設定する.
    ///
    void SetAxes(const PlotAxis& x_axis, const PlotAxis& y_axis);

    ///
    /// 表示するサンプル列を設定し, 再描画を要求する.
    ///
    /// 縦軸レンジは SetAxes() で設定した y 軸レンジを使用する.
    ///
    void SetSamples(std::span<const float> samples);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    ///
    /// プロット領域 (軸ラベルのマージンを除いた矩形) を求める.
    ///
    [[nodiscard]] auto PlotArea() const -> QRect;

    ///
    /// グリッドと目盛りラベルを描画する.
    ///
    void DrawAxes(QPainter* painter, const QRect& area) const;

    ///
    /// サンプル列の折れ線を描画する.
    ///
    void DrawSamples(QPainter* painter, const QRect& area) const;

    PlotAxis x_axis_;
    PlotAxis y_axis_;
    std::vector<float> samples_;
};
