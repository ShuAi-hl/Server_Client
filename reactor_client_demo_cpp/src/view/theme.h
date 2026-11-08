#pragma once
// theme.h - 深色主题：设计令牌（语义色 / 字号 / 控件尺寸）与全局 QSS
// 视图层统一从这里取色，避免散落的硬编码色值。
#include <QApplication>
#include <QColor>
#include <QFont>
#include <QPalette>
#include <QString>

namespace theme {

// ---- 语义色 ----
inline QColor surface() { return QColor("#0F1319"); }         // 窗口底色
inline QColor surfaceVariant() { return QColor("#171C24"); }  // 面板
inline QColor surfaceRaised() { return QColor("#1E242D"); }   // 控件
inline QColor primary() { return QColor("#3B82F6"); }
inline QColor success() { return QColor("#22C55E"); }
inline QColor danger() { return QColor("#EF4444"); }
inline QColor outline() { return QColor("#2A313C"); }
inline QColor text() { return QColor("#E6EDF3"); }
inline QColor textMuted() { return QColor("#8B949E"); }

// ---- 字号（modular scale，base 16px / 1.333）----
inline int fontDisplay() { return 28; }  // 大标题
inline int fontTitle() { return 21; }    // 区块标题
inline int fontBody() { return 16; }     // 正文
inline int fontCaption() { return 12; }  // 元信息

inline QString styleSheet() {
    return QStringLiteral(R"QSS(
QMainWindow, QDialog {
    background-color: #0F1319;
}
QWidget {
    color: #E6EDF3;
    font-family: "Microsoft YaHei UI", "Segoe UI", "PingFang SC", sans-serif;
    font-size: 16px;
}
QWidget#panel {
    background-color: #171C24;
    border: 1px solid #2A313C;
    border-radius: 10px;
}
QWidget#videoArea {
    background-color: #0B0E13;
    border: 1px solid #2A313C;
    border-radius: 10px;
}
QLabel#windowTitle {
    font-size: 28px;
    font-weight: 600;
    color: #E6EDF3;
}
QLabel#sectionTitle {
    font-size: 21px;
    font-weight: 600;
    color: #E6EDF3;
    padding: 2px 0;
}
QLabel#caption {
    font-size: 12px;
    color: #8B949E;
}
QLabel#statusLabel {
    font-size: 12px;
    color: #8B949E;
    padding: 0 10px;
}
QLabel#statusLabel[state="on"] { color: #22C55E; }
QLabel#statusLabel[state="off"] { color: #8B949E; }
QLabel#statusLabel[state="error"] { color: #EF4444; }

QPushButton {
    background-color: #1E242D;
    border: 1px solid #2A313C;
    border-radius: 8px;
    padding: 6px 14px;
    min-height: 40px;
    color: #E6EDF3;
}
QPushButton:hover { background-color: #242B36; border-color: #3B4250; }
QPushButton:pressed { background-color: #1A2029; }
QPushButton:focus { border: 1px solid #3B82F6; }
QPushButton:disabled {
    color: #5A6470;
    background-color: #14191F;
    border-color: #22272F;
}
QPushButton[variant="primary"] {
    background-color: #3B82F6;
    border-color: #3B82F6;
    color: #FFFFFF;
    font-weight: 600;
}
QPushButton[variant="primary"]:hover { background-color: #4B8FF7; }
QPushButton[variant="primary"]:pressed { background-color: #2F74E0; }
QPushButton[variant="primary"]:disabled {
    background-color: #23374F;
    border-color: #23374F;
    color: #6B7A8C;
}
QPushButton[variant="danger"] {
    background-color: #EF4444;
    border-color: #EF4444;
    color: #FFFFFF;
    font-weight: 600;
}
QPushButton[variant="danger"]:hover { background-color: #F35A5A; }
QPushButton[variant="danger"]:disabled {
    background-color: #3A2125;
    border-color: #3A2125;
    color: #7A5A5E;
}
QPushButton:checked {
    background-color: #3B82F6;
    border-color: #3B82F6;
    color: #FFFFFF;
}

QLineEdit {
    background-color: #0F1319;
    border: 1px solid #2A313C;
    border-radius: 8px;
    padding: 6px 10px;
    min-height: 36px;
    selection-background-color: #3B82F6;
    selection-color: #FFFFFF;
}
QLineEdit:focus { border: 1px solid #3B82F6; }
QLineEdit:disabled { color: #5A6470; }

QComboBox {
    background-color: #1E242D;
    border: 1px solid #2A313C;
    border-radius: 8px;
    padding: 2px 10px;
    min-height: 36px;
}
QComboBox:hover { border-color: #3B4250; }
QComboBox:focus { border: 1px solid #3B82F6; }
QComboBox:disabled {
    color: #5A6470;
    background-color: #14191F;
    border-color: #22272F;
}
QComboBox QAbstractItemView {
    background-color: #1E242D;
    border: 1px solid #2A313C;
    selection-background-color: #1D4ED8;
    selection-color: #FFFFFF;
    outline: none;
}

/* 音频电平表：绿色正常、接近削波转红 */
QProgressBar#levelMeter {
    background-color: #14191F;
    border: 1px solid #2A313C;
    border-radius: 5px;
    min-height: 10px;
    max-height: 10px;
}
QProgressBar#levelMeter::chunk {
    background-color: #22C55E;
    border-radius: 4px;
}
QProgressBar#levelMeter[hot="true"]::chunk {
    background-color: #EF4444;
}

QListWidget {
    background-color: transparent;
    border: none;
    outline: none;
}
QListWidget::item {
    padding: 8px;
    border-radius: 6px;
    color: #E6EDF3;
}
QListWidget::item:hover { background-color: #1E242D; }
QListWidget::item:selected { background-color: #1D4ED8; color: #FFFFFF; }

QTextBrowser {
    background-color: #0F1319;
    border: 1px solid #2A313C;
    border-radius: 8px;
    padding: 8px;
    font-size: 16px;
    selection-background-color: #3B82F6;
}

QStatusBar {
    background-color: #171C24;
    border-top: 1px solid #2A313C;
    min-height: 26px;
}
QStatusBar::item { border: none; }

QToolBar {
    background-color: #171C24;
    border: none;
    border-bottom: 1px solid #2A313C;
    padding: 6px;
    spacing: 8px;
}

QScrollBar:vertical {
    background: transparent;
    width: 10px;
    margin: 2px;
}
QScrollBar::handle:vertical {
    background: #2A313C;
    border-radius: 5px;
    min-height: 28px;
}
QScrollBar::handle:vertical:hover { background: #3B4250; }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: transparent; }

QToolTip {
    background-color: #1E242D;
    color: #E6EDF3;
    border: 1px solid #2A313C;
    padding: 6px;
}
)QSS");
}

// 应用深色调色板 + 全局 QSS
inline void apply(QApplication& app) {
    app.setStyle(QStringLiteral("Fusion"));

    QPalette pal = app.palette();
    pal.setColor(QPalette::Window, surface());
    pal.setColor(QPalette::WindowText, text());
    pal.setColor(QPalette::Base, surface());
    pal.setColor(QPalette::AlternateBase, surfaceVariant());
    pal.setColor(QPalette::Text, text());
    pal.setColor(QPalette::Button, surfaceRaised());
    pal.setColor(QPalette::ButtonText, text());
    pal.setColor(QPalette::Highlight, primary());
    pal.setColor(QPalette::HighlightedText, Qt::white);
    pal.setColor(QPalette::ToolTipBase, surfaceRaised());
    pal.setColor(QPalette::ToolTipText, text());
    pal.setColor(QPalette::Disabled, QPalette::WindowText, textMuted());
    pal.setColor(QPalette::Disabled, QPalette::Text, textMuted());
    pal.setColor(QPalette::Disabled, QPalette::ButtonText, textMuted());
    app.setPalette(pal);

    QFont font(QStringLiteral("Microsoft YaHei UI"));
    font.setPointSize(10);
    app.setFont(font);

    app.setStyleSheet(styleSheet());
}

}  // namespace theme
