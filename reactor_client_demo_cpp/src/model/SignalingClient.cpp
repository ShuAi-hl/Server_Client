// SignalingClient.cpp - TCP 信令通道实现
#include "SignalingClient.h"

#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonParseError>

SignalingClient::SignalingClient(QObject* parent) : QObject(parent) {
    socket_ = new QTcpSocket(this);

    connect(socket_, &QTcpSocket::connected, this, &SignalingClient::onConnected);
    connect(socket_, &QTcpSocket::disconnected, this, &SignalingClient::onDisconnected);
    connect(socket_, &QTcpSocket::readyRead, this, &SignalingClient::onReadyRead);
    connect(socket_, &QTcpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError) {
        emit connectionFailed(socket_->errorString());
    });

    // 15s 一次心跳保活，服务端空闲 60s 会清理会话
    heartbeatTimer_.setInterval(15000);
    connect(&heartbeatTimer_, &QTimer::timeout, this, &SignalingClient::sendPing);
}

void SignalingClient::connectToServer(const QString& host, quint16 port) {
    readBuffer_.clear();
    socket_->abort();
    socket_->connectToHost(host, port);
}

void SignalingClient::disconnectFromServer() {
    heartbeatTimer_.stop();
    socket_->disconnectFromHost();
}

bool SignalingClient::isConnected() const {
    return socket_->state() == QAbstractSocket::ConnectedState;
}

QString SignalingClient::localAddress() const {
    QHostAddress addr = socket_->localAddress();
    if (addr.protocol() == QAbstractSocket::IPv6Protocol) {
        bool ok = false;
        const quint32 v4 = addr.toIPv4Address(&ok);
        if (ok) addr = QHostAddress(v4);
    }
    return addr.toString();
}

void SignalingClient::onConnected() {
    emit connected();
    heartbeatTimer_.start();
}

void SignalingClient::onDisconnected() {
    heartbeatTimer_.stop();
    emit disconnected();
}

void SignalingClient::sendMessage(const ProtocolMessage& msg) {
    if (!isConnected()) {
        emit errorOccurred(QStringLiteral("未连接到服务器"));
        return;
    }
    QByteArray line = QJsonDocument(msg.toJson()).toJson(QJsonDocument::Compact);
    line.append('\n');
    socket_->write(line);
}

void SignalingClient::login(const QString& userId, const QString& userName) {
    userId_ = userId;
    userName_ = userName;
    ProtocolMessage m;
    m.type = "LOGIN";
    m.userId = userId;
    m.userName = userName;
    sendMessage(m);
}

void SignalingClient::registerUser(const QString& userId, const QString& userName) {
    userId_ = userId;
    userName_ = userName;
    ProtocolMessage m;
    m.type = "REGISTER";
    m.userId = userId;
    m.userName = userName;
    sendMessage(m);
}

void SignalingClient::createRoom(const QString& roomId, const QString& roomName) {
    ProtocolMessage m;
    m.type = "CREATE_ROOM";
    m.roomId = roomId;
    m.roomName = roomName;
    m.userId = userId_;
    m.userName = userName_;
    sendMessage(m);
}

void SignalingClient::joinRoom(const QString& roomId) {
    ProtocolMessage m;
    m.type = "JOIN_ROOM";
    m.roomId = roomId;
    sendMessage(m);
}

void SignalingClient::leaveRoom(const QString& roomId) {
    ProtocolMessage m;
    m.type = "LEAVE_ROOM";
    m.roomId = roomId;
    sendMessage(m);
}

void SignalingClient::listRooms() {
    ProtocolMessage m;
    m.type = "LIST_ROOMS";
    sendMessage(m);
}

void SignalingClient::queryRoom(const QString& roomId) {
    ProtocolMessage m;
    m.type = "ROOM_INFO";
    m.roomId = roomId;
    sendMessage(m);
}

void SignalingClient::sendRoomChat(const QString& roomId, const QString& content) {
    ProtocolMessage m;
    m.type = "ROOM_CHAT";
    m.roomId = roomId;
    m.content = content;
    m.userId = userId_;
    m.userName = userName_;
    sendMessage(m);
}

void SignalingClient::sendMediaRegister(const QString& roomId, const QString& ip, quint16 port) {
    ProtocolMessage m;
    m.type = "MEDIA_REGISTER";
    m.roomId = roomId;
    m.extra.insert("ip", ip);
    m.extra.insert("port", QString::number(port));
    sendMessage(m);
}

void SignalingClient::sendSdpOffer(const QString& roomId, const QString& sdp) {
    ProtocolMessage m;
    m.type = "SDP_OFFER";
    m.roomId = roomId;
    m.payload = sdp;
    m.userId = userId_;
    m.userName = userName_;
    sendMessage(m);
}

void SignalingClient::sendSdpAnswer(const QString& roomId, const QString& sdp) {
    ProtocolMessage m;
    m.type = "SDP_ANSWER";
    m.roomId = roomId;
    m.payload = sdp;
    m.userId = userId_;
    m.userName = userName_;
    sendMessage(m);
}

void SignalingClient::sendIceCandidate(const QString& roomId, const QString& candidate) {
    ProtocolMessage m;
    m.type = "ICE_CANDIDATE";
    m.roomId = roomId;
    m.payload = candidate;
    m.userId = userId_;
    m.userName = userName_;
    sendMessage(m);
}

void SignalingClient::logout() {
    ProtocolMessage m;
    m.type = "LOGOUT";
    m.userId = userId_;
    sendMessage(m);
    // 服务端会主动关闭连接，这里不再 disconnect，避免 TCP RST
}

void SignalingClient::sendPing() {
    if (!isConnected()) return;
    ProtocolMessage m;
    m.type = "PING";
    m.userId = userId_;
    sendMessage(m);
}

void SignalingClient::onReadyRead() {
    readBuffer_.append(socket_->readAll());

    // TCP 粘包/半包：按 '\n' 拆帧
    int pos;
    while ((pos = readBuffer_.indexOf('\n')) >= 0) {
        QByteArray line = readBuffer_.left(pos);
        readBuffer_.remove(0, pos + 1);
        if (line.endsWith('\r')) line.chop(1);
        if (line.trimmed().isEmpty()) continue;

        QJsonParseError err{};
        QJsonDocument doc = QJsonDocument::fromJson(line, &err);
        if (err.error != QJsonParseError::NoError || !doc.isObject()) {
            emit errorOccurred(QStringLiteral("服务端返回非法 JSON：%1").arg(err.errorString()));
            continue;
        }
        handleMessage(doc.object());
    }
}

void SignalingClient::handleMessage(const QJsonObject& obj) {
    const ProtocolMessage msg = ProtocolMessage::fromJson(obj);
    const QString type = msg.type;

    if (type == "LOGIN_OK" || type == "REGISTER_OK") {
        emit loginResult(msg.success, msg.content);
    } else if (type == "ROOM_CREATED") {
        emit roomCreated(msg.success, msg.roomId, msg.content);
    } else if (type == "ROOM_INFO") {
        // JOIN_ROOM 与 ROOM_INFO 查询共用该响应类型
        const QJsonObject roomInfo = obj.value("roomInfo").toObject();
        emit roomJoined(msg.success, roomInfo, msg.content);
        emit roomInfoReceived(msg.success, roomInfo, msg.content);
    } else if (type == "LEAVE_ROOM_OK") {
        emit roomLeft(msg.success, msg.content);
    } else if (type == "ROOM_LIST") {
        emit roomListReceived(obj.value("rooms").toArray());
    } else if (type == "ROOM_MESSAGE") {
        emit chatReceived(msg.userId, msg.userName, msg.roomId, msg.content, msg.timestamp);
    } else if (type == "MEDIA_REGISTER_OK") {
        emit mediaRegistered(msg.success, msg.content);
    } else if (type == "PEER_SDP_OFFER") {
        emit peerSdp("OFFER", msg.userId, msg.payload);
    } else if (type == "PEER_SDP_ANSWER") {
        emit peerSdp("ANSWER", msg.userId, msg.payload);
    } else if (type == "PEER_ICE_CANDIDATE") {
        emit peerIceCandidate(msg.userId, msg.payload);
    } else if (type == "PONG") {
        emit pong();
    } else if (type == "ERROR") {
        emit serverMessage(type, msg.content);
    } else {
        emit serverMessage(type, msg.content);
    }
}
