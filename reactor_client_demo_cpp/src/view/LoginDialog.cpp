// LoginDialog.cpp - 登录对话框实现
#include "LoginDialog.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QVBoxLayout>

#include "theme.h"

LoginDialog::LoginDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle(QStringLiteral("连接到会议服务器"));
    setModal(true);
    setMinimumWidth(460);

    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(28, 24, 28, 24);
    root->setSpacing(16);

    QLabel* title = new QLabel(QStringLiteral("加入会议"), this);
    title->setObjectName("windowTitle");
    root->addWidget(title);

    QLabel* subtitle = new QLabel(
        QStringLiteral("填写服务器地址与用户身份，连接成功后即可创建或加入房间。"), this);
    subtitle->setObjectName("caption");
    subtitle->setWordWrap(true);
    root->addWidget(subtitle);

    QFormLayout* form = new QFormLayout();
    form->setSpacing(12);
    form->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    hostEdit_ = new QLineEdit(QStringLiteral("127.0.0.1"), this);
    hostEdit_->setPlaceholderText(QStringLiteral("服务器 IP 或主机名"));

    portEdit_ = new QLineEdit(QStringLiteral("8888"), this);
    portEdit_->setPlaceholderText(QStringLiteral("TCP 端口（默认 8888）"));
    portEdit_->setMaximumWidth(140);

    userEdit_ = new QLineEdit(this);
    userEdit_->setPlaceholderText(QStringLiteral("登录 ID，例如 u1001"));

    nameEdit_ = new QLineEdit(this);
    nameEdit_->setPlaceholderText(QStringLiteral("显示名称，例如 张三"));

    form->addRow(QStringLiteral("服务器地址"), hostEdit_);
    form->addRow(QStringLiteral("端口"), portEdit_);
    form->addRow(QStringLiteral("用户 ID"), userEdit_);
    form->addRow(QStringLiteral("用户名"), nameEdit_);
    root->addLayout(form);

    errorLabel_ = new QLabel(this);
    errorLabel_->setObjectName("statusLabel");
    errorLabel_->setProperty("state", "error");
    errorLabel_->setWordWrap(true);
    errorLabel_->hide();
    root->addWidget(errorLabel_);

    root->addStretch();

    QHBoxLayout* buttons = new QHBoxLayout();
    buttons->addStretch();

    QPushButton* cancelButton = new QPushButton(QStringLiteral("取消"), this);
    connectButton_ = new QPushButton(QStringLiteral("连接"), this);
    connectButton_->setProperty("variant", "primary");
    connectButton_->setDefault(true);
    buttons->addWidget(cancelButton);
    buttons->addWidget(connectButton_);
    root->addLayout(buttons);

    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(connectButton_, &QPushButton::clicked, this, &LoginDialog::tryAccept);
    connect(userEdit_, &QLineEdit::returnPressed, this, &LoginDialog::tryAccept);
    connect(nameEdit_, &QLineEdit::returnPressed, this, &LoginDialog::tryAccept);
    connect(hostEdit_, &QLineEdit::returnPressed, this, &LoginDialog::tryAccept);
}

LoginDialog::Credentials LoginDialog::credentials() const {
    Credentials c;
    c.host = hostEdit_->text().trimmed();
    c.port = static_cast<quint16>(portEdit_->text().trimmed().toUInt());
    c.userId = userEdit_->text().trimmed();
    c.userName = nameEdit_->text().trimmed();
    return c;
}

void LoginDialog::setCredentials(const Credentials& credentials) {
    if (!credentials.host.isEmpty()) hostEdit_->setText(credentials.host);
    if (credentials.port != 0) portEdit_->setText(QString::number(credentials.port));
    if (!credentials.userId.isEmpty()) userEdit_->setText(credentials.userId);
    if (!credentials.userName.isEmpty()) nameEdit_->setText(credentials.userName);
}

void LoginDialog::showError(const QString& message) {
    errorLabel_->setText(message);
    errorLabel_->show();
}

void LoginDialog::tryAccept() {
    const QString host = hostEdit_->text().trimmed();
    if (host.isEmpty()) {
        showError(QStringLiteral("请填写服务器地址"));
        hostEdit_->setFocus();
        return;
    }

    bool ok = false;
    const uint port = portEdit_->text().trimmed().toUInt(&ok);
    if (!ok || port == 0 || port > 65535) {
        showError(QStringLiteral("端口必须是 1 - 65535 之间的整数"));
        portEdit_->setFocus();
        return;
    }

    const QString userId = userEdit_->text().trimmed();
    if (userId.isEmpty()) {
        showError(QStringLiteral("请填写用户 ID"));
        userEdit_->setFocus();
        return;
    }
    if (userId.contains(QRegularExpression(QStringLiteral("\\s")))) {
        showError(QStringLiteral("用户 ID 不能包含空白字符"));
        userEdit_->setFocus();
        return;
    }

    errorLabel_->hide();
    accept();
}
