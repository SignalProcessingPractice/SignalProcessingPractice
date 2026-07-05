///
/// @file PlotWidget.cpp
///

#include "view/PlotWidget.h"

#include <QFontMetrics>
#include <QPainter>
#include <QPen>
#include <QPolygonF>
#include <algorithm>
#include <cstddef>

namespace {

constexpr QColor kBackgroundColor{0x10, 0x10, 0x10};
constexpr QColor kGridColor{0x38, 0x38, 0x38};
constexpr QColor kLabelColor{0xA0, 0xA0, 0xA0};
constexpr QColor kLineColor{0x00, 0xC8, 0x50};
constexpr qreal kLineWidth = 1.0;

///
/// 軸ラベル用のマージン.
///
constexpr int kMarginLeft = 56;
constexpr int kMarginRight = 12;
constexpr int kMarginTop = 8;
constexpr int kMarginBottom = 20;

///
/// ラベルとプロット領域の間隔.
///
constexpr int kLabelGap = 4;

///
/// ラベルのフォントサイズ.
///
constexpr int kLabelFontSize = 7;

///
/// 目盛り値の表示有効桁数.
///
constexpr int kTickPrecision = 3;

///
/// 目盛り値をラベル文字列へ変換する (最大値側の目盛りには単位を付記).
///
auto FormatTick(float value, const QString& unit, bool is_max_tick) -> QString {
    QString text = QString::number(value, 'g', kTickPrecision);
    if (is_max_tick && !unit.isEmpty()) {
        text += QStringLiteral(" ") + unit;
    }
    return text;
}

}  // namespace

PlotWidget::PlotWidget(QWidget* parent) : QWidget(parent) {
}

void PlotWidget::SetAxes(const PlotAxis& x_axis, const PlotAxis& y_axis) {
    x_axis_ = x_axis;
    y_axis_ = y_axis;
    update();
}

void PlotWidget::SetSamples(std::span<const float> samples) {
    samples_.assign(samples.begin(), samples.end());
    update();
}

void PlotWidget::paintEvent([[maybe_unused]] QPaintEvent* event) {
    QPainter painter{this};
    painter.fillRect(rect(), kBackgroundColor);

    QFont label_font = font();
    label_font.setPointSize(kLabelFontSize);
    painter.setFont(label_font);

    const QRect area = PlotArea();
    if (!area.isValid()) {
        return;
    }

    DrawAxes(&painter, area);
    DrawSamples(&painter, area);
}

auto PlotWidget::PlotArea() const -> QRect {
    return rect().adjusted(kMarginLeft, kMarginTop, -kMarginRight, -kMarginBottom);
}

void PlotWidget::DrawAxes(QPainter* painter, const QRect& area) const {
    const QFontMetrics metrics{painter->font()};

    // 縦軸 (y): 下から上へ min → max.
    for (int tick = 0; tick < y_axis_.tick_count; ++tick) {
        const qreal fraction = static_cast<qreal>(tick) / (y_axis_.tick_count - 1);
        const auto y_pos = static_cast<int>(area.bottom() - (fraction * area.height()));
        const float value = y_axis_.min_value + (static_cast<float>(fraction) *
                                                 (y_axis_.max_value - y_axis_.min_value));

        painter->setPen(QPen{kGridColor, 1.0});
        painter->drawLine(area.left(), y_pos, area.right(), y_pos);

        const QString label = FormatTick(value, y_axis_.unit, tick == y_axis_.tick_count - 1);
        const QRect label_rect{0, y_pos - (metrics.height() / 2), area.left() - kLabelGap,
                               metrics.height()};
        painter->setPen(kLabelColor);
        painter->drawText(label_rect, Qt::AlignRight | Qt::AlignVCenter, label);
    }

    // 横軸 (x): 左から右へ min → max.
    for (int tick = 0; tick < x_axis_.tick_count; ++tick) {
        const qreal fraction = static_cast<qreal>(tick) / (x_axis_.tick_count - 1);
        const auto x_pos = static_cast<int>(area.left() + (fraction * area.width()));
        const float value = x_axis_.min_value + (static_cast<float>(fraction) *
                                                 (x_axis_.max_value - x_axis_.min_value));

        painter->setPen(QPen{kGridColor, 1.0});
        painter->drawLine(x_pos, area.top(), x_pos, area.bottom());

        const QString label = FormatTick(value, x_axis_.unit, tick == x_axis_.tick_count - 1);
        const int label_width = metrics.horizontalAdvance(label);

        // ラベルがウィジェット右端からはみ出さないようにクランプする.
        const int label_x = std::min(x_pos - (label_width / 2), rect().right() - label_width);
        const QRect label_rect{label_x, area.bottom() + kLabelGap, label_width, metrics.height()};
        painter->setPen(kLabelColor);
        painter->drawText(label_rect, Qt::AlignHCenter | Qt::AlignTop, label);
    }
}

void PlotWidget::DrawSamples(QPainter* painter, const QRect& area) const {
    if (samples_.size() < 2 || y_axis_.max_value <= y_axis_.min_value) {
        return;
    }

    painter->save();
    painter->setClipRect(area);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen{kLineColor, kLineWidth});

    const auto plot_width = static_cast<qreal>(area.width());
    const auto plot_height = static_cast<qreal>(area.height());
    const auto count = samples_.size();
    const auto value_range = y_axis_.max_value - y_axis_.min_value;

    QPolygonF polyline;
    polyline.reserve(static_cast<qsizetype>(count));
    for (std::size_t i = 0; i < count; ++i) {
        const qreal x_pos =
                area.left() + (plot_width * static_cast<qreal>(i) / static_cast<qreal>(count - 1));
        const auto normalized = static_cast<qreal>((samples_[i] - y_axis_.min_value) / value_range);
        const qreal y_pos = area.top() + (plot_height * (1.0 - normalized));
        polyline.append(QPointF{x_pos, y_pos});
    }
    painter->drawPolyline(polyline);
    painter->restore();
}
