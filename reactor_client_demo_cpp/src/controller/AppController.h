#pragma once
// AppController.h - 控制器（MVC 中的 C）
// 持有 SignalingClient / MediaEngine / AppModel，把界面动作翻译成业务操作，
// 再把模型与媒体的变化以信号形式抛给视图。
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QStringList>

#include "AppModel.h"

class QImage;
class SignalingClient;
class MediaEngine;

class AppController : public QObject {
    Q_OBJECT
public:
    explicit AppController(QObject* parent = nullptr);
    ~AppController() override;

    AppModel* model() const { return model_; }

    // ---- 硬件设备查询（供视图填充下拉框）----
    QStringList cameraDeviceNames() const;
    QStringList audioInputDeviceNames() const;
    QStringList audioOutputDeviceNames() const;
    int cameraDeviceIndex() const;
    int audioInputDeviceIndex() const;
    int audioOutputDeviceIndex() const;

public slots:
    // ---- 连接 ----
    void connectToServer(const QString& host, quint16 port,
                         const QString& userId, const QString& userName);
    void disconnectFromServer();

    // ---- 房间 ----
    void refreshRooms();
    void createRoom(const QString& roomId, const QString& roomName);
    void joinRoom(const QString& roomId);
    void leaveRoom();

    // ---- 聊天 ----
    void sendChat(const QString& text);

    // ---- 媒体 ----
    void toggleVideo(bool on);
    void toggleMic(bool on);
    void toggleSpeakerMuted(bool muted);

    // 切换到指定摄像头 / 麦克风（采集进行中会立即重启采集）
    void selectCameraDevice(int index);
    void selectAudioInputDevice(int index);
    void selectAudioOutputDevice(int index);

signals:
    void localFrame(const QImage& image);
    void remoteFrame(const QImage& image);
    void errorOccurred(const QString& message);
    // 音频电平（0.0~1.0），供界面电平表使用
    void audioLevelsChanged(double inputLevel, double outputLevel);

private slots:
    void onSignalingConnected();
    void onSignalingDisconnected();
    void onConnectionFailed(const QString& reason);
    void onLoginResult(bool success, const QString& message);
    void onRoomCreated(bool success, const QString& roomId, const QString& message);
    void onRoomJoined(bool success, const QJsonObject& roomInfo, const QString& message);
    void onRoomLeft(bool success, const QString& message);
    void onRoomList(const QJsonArray& rooms);
    void onChatReceived(const QString& userId, const QString& userName,
                        const QString& roomId, const QString& content, qint64 timestamp);
    void onMediaRegistered(bool success, const QString& message);
    void onServerMessage(const QString& type, const QString& content);
    void onSignalingError(const QString& message);
    void onMediaStats(double localFps, int localKbps, double remoteFps, int remoteKbps);
    void onMediaWarning(const QString& message);
    void onVideoSendStateChanged(bool on);
    void onAudioSendStateChanged(bool on);

private:
    void registerMedia();

    AppModel* model_ = nullptr;
    SignalingClient* signaling_ = nullptr;
    MediaEngine* media_ = nullptr;

    QString host_;
    quint16 port_ = 8888;
};
