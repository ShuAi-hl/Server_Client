// VideoWidget.cpp - 视频显示控件实现
#include "VideoWidget.h"

#include <QFont>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPen>

#include "theme.h"

VideoWidget::VideoWidget(const QString& title, QWidget* parent)
    : QWidget(parent), title_(title) {
    setMinimumSize(320, 180);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void VideoWidget::setFrame(const QImage& image) {
    if (image.isNull()) return;
    frame_ = image;
    update();
}

void VideoWidget::clearFrame() {
    if (frame_.isNull()) return;
    frame_ = QImage();
    update();
}

void VideoWidget::setPlaceholderText(const QString& text) {
    if (placeholder_ == text) return;
    placeholder_ = text;
    update();
}

void VideoWidget::setBadge(const QString& text, bool active) {
    if (badge_ == text && badgeActive_ == active) return;
    badge_ = text;
    badgeActive_ = active;
    update();
}

void VideoWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QRectF bounds = rect();

    // 圆角底色
    QPainterPath clip;
    clip.addRoundedRect(bounds.adjusted(0.5, 0.5, -0.5, -0.5), 10, 10);
    painter.fillPath(clip, QColor("#0B0E13"));

    if (!frame_.isNull()) {
        // 等比缩放并居中（留黑边），避免画面拉伸变形
        const QSizeF scaled = QSizeF(frame_.size()).scaled(bounds.size(), Qt::KeepAspectRatio);
        const QRectF target(bounds.center().x() - scaled.width() / 2.0,
                            bounds.center().y() - scaled.height() / 2.0,
                            scaled.width(), scaled.height());
        painter.setClipPath(clip);
        painter.drawImage(target, frame_);
        painter.setClipping(false);
    } else {
        QFont font = painter.font();
        font.setPixelSize(theme::fontTitle());
        painter.setFont(font);
        painter.setPen(theme::textMuted());
        painter.drawText(bounds, Qt::AlignCenter, placeholder_);
    }

    // 左上角标题
    QFont titleFont = painter.font();
    titleFont.setPixelSize(theme::fontCaption());
    titleFont.setWeight(QFont::DemiBold);
    painter.setFont(titleFont);

    QFontMetrics metrics(titleFont);
    const QRectF titleRect(12, 10, metrics.horizontalAdvance(title_) + 16, 24);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(15, 19, 25, 190));
    painter.drawRoundedRect(titleRect, 6, 6);
    painter.setPen(theme::text());
    painter.drawText(titleRect, Qt::AlignCenter, title_);

    // 右上角状态徽标
    if (!badge_.isEmpty()) {
        const QColor badgeColor = badgeActive_ ? theme::success() : theme::textMuted();
        QFont badgeFont = painter.font();
        badgeFont.setPixelSize(theme::fontCaption());
        painter.setFont(badgeFont);
        QFontMetrics badgeMetrics(badgeFont);
        const QRectF badgeRect(bounds.width() - badgeMetrics.horizontalAdvance(badge_) - 28, 10,
                               badgeMetrics.horizontalAdvance(badge_) + 16, 24);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(15, 19, 25, 190));
        painter.drawRoundedRect(badgeRect, 6, 6);
        painter.setPen(badgeColor);
        painter.drawText(badgeRect, Qt::AlignCenter, badge_);
    }

    // 边框
    painter.setPen(QPen(theme::outline(), 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(bounds.adjusted(0.5, 0.5, -0.5, -0.5), 10, 10);
}
