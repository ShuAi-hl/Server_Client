#pragma once
// MainWindow.h - 主窗口（MVC 中的 V）
// 只负责展示模型状态、把用户操作以信号转发给控制器，不直接发起网络或媒体调用。
#include <QMainWindow>
#include <QString>
#include <QStringList>

class AppModel;
class QImage;
class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
class QTextBrowser;
class VideoWidget;
class QComboBox;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(AppModel* model, QWidget* parent = nullptr);

public slots:
    void showLoginDialog();
    void setLocalFrame(const QImage& image);
    void setRemoteFrame(const QImage& image);
    void showError(const QString& message);
    // 由装配层注入可选的硬件设备列表与当前选择
    void setMediaDevices(const QStringList& cameras, int cameraIndex,
                         const QStringList& microphones, int micIndex);

signals:
    void loginRequested(const QString& host, quint16 port,
                        const QString& userId, const QString& userName);
    void disconnectRequested();
    void refreshRoomsRequested();
    void createRoomRequested(const QString& roomId, const QString& roomName);
    void joinRoomRequested(const QString& roomId);
    void leaveRoomRequested();
    void sendChatRequested(const QString& text);
    void videoToggleRequested(bool on);
    void micToggleRequested(bool on);
    void speakerMuteToggleRequested(bool muted);
    void cameraDeviceSelected(int index);
    void micDeviceSelected(int index);

private slots:
    void onConnStateChanged();
    void onRoomsChanged();
    void onRoomChanged();
    void onMembersChanged();
    void onChatChanged();
    void onMediaStateChanged();
    void onMediaStatsChanged();
    void onStatusTextChanged(const QString& text);
    void onRefreshClicked();
    void onCreateClicked();
    void onJoinClicked();
    void onLeaveClicked();
    void onSendClicked();

private:
    void buildUi();
    void buildStatusBar();
    void connectModelSignals();
    void updateActions();
    QString selectedRoomId() const;

    AppModel* model_ = nullptr;

    QListWidget* roomList_ = nullptr;
    QListWidget* memberList_ = nullptr;
    QTextBrowser* chatView_ = nullptr;
    QLineEdit* chatInput_ = nullptr;

    QPushButton* connectButton_ = nullptr;
    QPushButton* disconnectButton_ = nullptr;
    QPushButton* refreshButton_ = nullptr;
    QPushButton* createButton_ = nullptr;
    QPushButton* joinButton_ = nullptr;
    QPushButton* leaveButton_ = nullptr;
    QPushButton* sendButton_ = nullptr;

    QPushButton* videoButton_ = nullptr;
    QPushButton* micButton_ = nullptr;
    QPushButton* speakerButton_ = nullptr;

    QComboBox* cameraCombo_ = nullptr;
    QComboBox* micCombo_ = nullptr;
    // 程序化填充下拉框时抑制 currentIndexChanged，避免误触发设备切换
    bool updatingDeviceCombos_ = false;

    VideoWidget* localView_ = nullptr;
    VideoWidget* remoteView_ = nullptr;

    QLabel* statusConn_ = nullptr;
    QLabel* statusRoom_ = nullptr;
    QLabel* statusMembers_ = nullptr;
    QLabel* statusMedia_ = nullptr;
    QLabel* statusStats_ = nullptr;
};
