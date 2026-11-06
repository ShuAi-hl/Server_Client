#pragma once
// LoginDialog.h - 登录对话框：采集服务器地址与用户身份，带输入校验
#include <QDialog>
#include <QString>

class QLabel;
class QLineEdit;
class QPushButton;

class LoginDialog : public QDialog {
    Q_OBJECT
public:
    struct Credentials {
        QString host = QStringLiteral("127.0.0.1");
        quint16 port = 8888;
        QString userId;
        QString userName;
    };

    explicit LoginDialog(QWidget* parent = nullptr);

    Credentials credentials() const;
    void setCredentials(const Credentials& credentials);

private slots:
    void tryAccept();

private:
    void showError(const QString& message);

    QLineEdit* hostEdit_ = nullptr;
    QLineEdit* portEdit_ = nullptr;
    QLineEdit* userEdit_ = nullptr;
    QLineEdit* nameEdit_ = nullptr;
    QLabel* errorLabel_ = nullptr;
    QPushButton* connectButton_ = nullptr;
};
