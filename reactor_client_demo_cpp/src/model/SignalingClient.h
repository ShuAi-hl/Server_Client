#pragma once
// SignalingClient.h - TCP 信令通道
// 负责：连接服务器、按行拆帧（粘包/半包）、收发 JSON、心跳 PING
#include <QByteArray>
#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QTimer>

#include "Protocol.h"

class SignalingClient : public QObject {
    Q_OBJECT
public:
    explicit SignalingClient(QObject* parent = nullptr);

    void connectToServer(const QString& host, quint16 port);
    void disconnectFromServer();
    bool isConnected() const;
    // 本机与服务器通信所使用的本地 IP（MEDIA_REGISTER 使用，IPv6 映射地址会转成 IPv4）
    QString localAddress() const;

    QString userId() const { return userId_; }
    QString userName() const { return userName_; }

    // ---- 信令发送 ----
    void login(const QString& userId, const QString& userName);
    void registerUser(const QString& userId, const QString& userName);
    void createRoom(const QString& roomId, const QString& roomName);
    void joinRoom(const QString& roomId);
    void leaveRoom(const QString& roomId);
    void listRooms();
    void queryRoom(const QString& roomId);
    void sendRoomChat(const QString& roomId, const QString& content);
    void sendMediaRegister(const QString& roomId, const QString& ip, quint16 port);
    void sendSdpOffer(const QString& roomId, const QString& sdp);
    void sendSdpAnswer(const QString& roomId, const QString& sdp);
    void sendIceCandidate(const QString& roomId, const QString& candidate);
    void logout();

signals:
    void connected();
    void disconnected();
    void connectionFailed(const QString& reason);
    // 服务端响应
    void loginResult(bool success, const QString& message);
    void roomCreated(bool success, const QString& roomId, const QString& message);
    void roomJoined(bool success, const QJsonObject& roomInfo, const QString& message);
    void roomLeft(bool success, const QString& message);
    void roomListReceived(const QJsonArray& rooms);
    void roomInfoReceived(bool success, const QJsonObject& roomInfo, const QString& message);
    void chatReceived(const QString& userId, const QString& userName,
                      const QString& roomId, const QString& content, qint64 timestamp);
    void mediaRegistered(bool success, const QString& message);
    void peerSdp(const QString& kind, const QString& userId, const QString& payload);
    void peerIceCandidate(const QString& userId, const QString& payload);
    void pong();
    // 未知/错误消息
    void serverMessage(const QString& type, const QString& content);
    void errorOccurred(const QString& message);

private slots:
    void onReadyRead();
    void onConnected();
    void onDisconnected();
    void sendPing();

private:
    void sendMessage(const ProtocolMessage& msg);
    void handleMessage(const QJsonObject& obj);

    QTcpSocket* socket_ = nullptr;
    QByteArray readBuffer_;
    QTimer heartbeatTimer_;
    QString userId_;
    QString userName_;
};
