#pragma once
// VideoWidget.h - 视频显示控件：等比缩放绘制一帧图像，无画面时显示占位提示
#include <QImage>
#include <QString>
#include <QWidget>

class VideoWidget : public QWidget {
    Q_OBJECT
public:
    explicit VideoWidget(const QString& title, QWidget* parent = nullptr);

    void setFrame(const QImage& image);
    void clearFrame();
    void setPlaceholderText(const QString& text);
    void setBadge(const QString& text, bool active);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QString title_;
    QString placeholder_ = QStringLiteral("无画面");
    QString badge_;
    QImage frame_;
    bool badgeActive_ = false;
};
