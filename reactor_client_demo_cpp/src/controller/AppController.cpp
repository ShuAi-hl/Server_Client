// AppController.cpp - 控制器实现：编排信令、媒体与模型
#include "AppController.h"

#include <QDateTime>
#include <QJsonArray>
#include <QJsonValue>
#include <QSettings>

#include "MediaEngine.h"
#include "SignalingClient.h"

namespace {
// reactor_server 的 UDP 媒体中继端口（服务端固定监听 10001）
constexpr quint16 kMediaRelayPort = 10001;

// 记住用户上次选择的硬件（QSettings 键名）
constexpr auto kKeyCameraIndex = "media/cameraIndex";
constexpr auto kKeyAudioInputIndex = "media/audioInputIndex";
}  // namespace

AppController::AppController(QObject* parent) : QObject(parent) {
    model_ = new AppModel(this);
    signaling_ = new SignalingClient(this);
    media_ = new MediaEngine(this);

    // 信令 -> 控制器
    connect(signaling_, &SignalingClient::connected, this, &AppController::onSignalingConnected);
    connect(signaling_, &SignalingClient::disconnected,
            this, &AppController::onSignalingDisconnected);
    connect(signaling_, &SignalingClient::connectionFailed,
            this, &AppController::onConnectionFailed);
    connect(signaling_, &SignalingClient::loginResult, this, &AppController::onLoginResult);
    connect(signaling_, &SignalingClient::roomCreated, this, &AppController::onRoomCreated);
    connect(signaling_, &SignalingClient::roomJoined, this, &AppController::onRoomJoined);
    connect(signaling_, &SignalingClient::roomLeft, this, &AppController::onRoomLeft);
    connect(signaling_, &SignalingClient::roomListReceived, this, &AppController::onRoomList);
    connect(signaling_, &SignalingClient::chatReceived, this, &AppController::onChatReceived);
    connect(signaling_, &SignalingClient::mediaRegistered, this, &AppController::onMediaRegistered);
    connect(signaling_, &SignalingClient::serverMessage, this, &AppController::onServerMessage);
    connect(signaling_, &SignalingClient::errorOccurred, this, &AppController::onSignalingError);

    // 媒体 -> 控制器
    connect(media_, &MediaEngine::localFrame, this, &AppController::localFrame);
    connect(media_, &MediaEngine::remoteFrame, this, &AppController::remoteFrame);
    connect(media_, &MediaEngine::statsChanged, this, &AppController::onMediaStats);
    connect(media_, &MediaEngine::warning, this, &AppController::onMediaWarning);
    connect(media_, &MediaEngine::videoSendStateChanged,
            this, &AppController::onVideoSendStateChanged);
    connect(media_, &MediaEngine::audioSendStateChanged,
            this, &AppController::onAudioSendStateChanged);

    // 恢复上次选择的摄像头 / 麦克风（设备数量变化时回退到第一个）
    QSettings settings(QStringLiteral("ReactorClient"), QStringLiteral("reactor_client"));
    const QStringList cams = MediaEngine::cameraDeviceNames();
    const QStringList mics = MediaEngine::audioInputDeviceNames();

    int camIndex = settings.value(kKeyCameraIndex, -1).toInt();
    if (camIndex >= cams.size()) camIndex = cams.isEmpty() ? -1 : 0;
    int micIndex = settings.value(kKeyAudioInputIndex, -1).toInt();
    if (micIndex >= mics.size()) micIndex = mics.isEmpty() ? -1 : 0;

    media_->setCameraDeviceIndex(camIndex);
    media_->setAudioInputDeviceIndex(micIndex);
}

QStringList AppController::cameraDeviceNames() const {
    return MediaEngine::cameraDeviceNames();
}

QStringList AppController::audioInputDeviceNames() const {
    return MediaEngine::audioInputDeviceNames();
}

int AppController::cameraDeviceIndex() const { return media_->cameraDeviceIndex(); }

int AppController::audioInputDeviceIndex() const { return media_->audioInputDeviceIndex(); }

void AppController::selectCameraDevice(int index) {
    if (index < 0) return;
    media_->setCameraDeviceIndex(index);
    QSettings(QStringLiteral("ReactorClient"), QStringLiteral("reactor_client"))
        .setValue(kKeyCameraIndex, index);
}

void AppController::selectAudioInputDevice(int index) {
    if (index < 0) return;
    media_->setAudioInputDeviceIndex(index);
    QSettings(QStringLiteral("ReactorClient"), QStringLiteral("reactor_client"))
        .setValue(kKeyAudioInputIndex, index);
}

AppController::~AppController() {
    media_->stopAll();
}

// ============================ 连接 ============================

void AppController::connectToServer(const QString& host, quint16 port,
                                    const QString& userId, const QString& userName) {
    host_ = host;
    port_ = port;

    model_->setServerInfo(host, port);
    model_->setSelfUser(userId, userName);
    model_->setConnState(AppModel::ConnState::Connecting);
    model_->setStatusText(QStringLiteral("正在连接 %1:%2 ...").arg(host).arg(port));

    signaling_->connectToServer(host, port);
}

void AppController::onSignalingConnected() {
    model_->setConnState(AppModel::ConnState::Connected);

    if (!media_->start()) {
        emit errorOccurred(QStringLiteral("本地 UDP 端口绑定失败，媒体功能不可用"));
    } else {
        model_->setMediaPort(media_->localPort());
        media_->setRelayTarget(host_, kMediaRelayPort);
        media_->setRegisterInfo(signaling_->localAddress(), QString());
        model_->setStatusText(
            QStringLiteral("已连接服务器，本地媒体端口 %1").arg(media_->localPort()));
    }

    model_->setAudioDevices(MediaEngine::hasAudioInput(), MediaEngine::hasAudioOutput());

    signaling_->login(model_->selfUserId(), model_->selfUserName());
}

void AppController::onSignalingDisconnected() {
    media_->stopAll();
    model_->setConnState(AppModel::ConnState::Disconnected);
    model_->setMediaPort(0);
    model_->setVideoSendEnabled(false);
    model_->setMicEnabled(false);
    model_->leaveRoom();
    model_->setStatusText(QStringLiteral("与服务器的连接已断开"));
}

void AppController::onConnectionFailed(const QString& reason) {
    if (model_->connState() == AppModel::ConnState::Disconnected) return;
    model_->setConnState(AppModel::ConnState::Disconnected);
    model_->setStatusText(QStringLiteral("连接失败：%1").arg(reason));
    emit errorOccurred(QStringLiteral("连接服务器失败：%1").arg(reason));
}

void AppController::disconnectFromServer() {
    if (signaling_->isConnected()) {
        signaling_->logout();
        signaling_->disconnectFromServer();
    }
    media_->stopAll();
    model_->setConnState(AppModel::ConnState::Disconnected);
    model_->leaveRoom();
    model_->setStatusText(QStringLiteral("已主动断开连接"));
}

void AppController::onLoginResult(bool success, const QString& message) {
    if (!success) {
        emit errorOccurred(QStringLiteral("登录失败：%1").arg(message));
        model_->setStatusText(QStringLiteral("登录失败：%1").arg(message));
        return;
    }
    model_->setConnState(AppModel::ConnState::LoggedIn);
    model_->setStatusText(QStringLiteral("登录成功：%1").arg(model_->selfUserName()));
    registerMedia();
    refreshRooms();
}

// ============================ 房间 ============================

void AppController::refreshRooms() {
    if (!model_->isLoggedIn()) return;
    signaling_->listRooms();
}

void AppController::createRoom(const QString& roomId, const QString& roomName) {
    if (!model_->isLoggedIn()) {
        emit errorOccurred(QStringLiteral("请先登录"));
        return;
    }
    if (roomId.trimmed().isEmpty()) {
        emit errorOccurred(QStringLiteral("房间 ID 不能为空"));
        return;
    }
    signaling_->createRoom(roomId.trimmed(), roomName.trimmed());
}

void AppController::joinRoom(const QString& roomId) {
    if (!model_->isLoggedIn()) {
        emit errorOccurred(QStringLiteral("请先登录"));
        return;
    }
    if (roomId.isEmpty()) {
        emit errorOccurred(QStringLiteral("请先选择一个房间"));
        return;
    }
    signaling_->joinRoom(roomId);
}

void AppController::leaveRoom() {
    if (!model_->isLoggedIn() || !model_->inRoom()) return;
    signaling_->leaveRoom(model_->currentRoomId());
}

void AppController::onRoomCreated(bool success, const QString& roomId, const QString& message) {
    if (!success) {
        emit errorOccurred(QStringLiteral("创建房间失败：%1").arg(message));
        return;
    }
    model_->setStatusText(QStringLiteral("房间已创建：%1").arg(roomId));
    refreshRooms();
    // 服务端已把创建者加入房间，这里再走一次 JOIN 以获取完整成员信息
    signaling_->joinRoom(roomId);
}

void AppController::onRoomJoined(bool success, const QJsonObject& roomInfo,
                                 const QString& message) {
    if (!success) {
        emit errorOccurred(QStringLiteral("加入房间失败：%1").arg(message));
        return;
    }

    const QString roomId = roomInfo.value("roomId").toString();
    const QString roomName = roomInfo.value("roomName").toString();
    const QString ownerId = roomInfo.value("ownerId").toString();

    QVector<MemberItem> members;
    const QJsonArray memberArray = roomInfo.value("members").toArray();
    members.reserve(memberArray.size());
    for (const QJsonValue& value : memberArray) {
        const QJsonObject obj = value.toObject();
        MemberItem item;
        item.userId = obj.value("userId").toString();
        item.userName = obj.value("userName").toString();
        members.append(item);
    }

    model_->enterRoom(roomId, roomName, ownerId);
    model_->setMembers(members);
    model_->setStatusText(QStringLiteral("已进入房间：%1").arg(roomId));

    // 进入房间后必须重新注册媒体地址，服务端按注册时的 roomId 转发
    registerMedia();
}

void AppController::onRoomLeft(bool success, const QString& message) {
    if (!success) {
        emit errorOccurred(QStringLiteral("离开房间失败：%1").arg(message));
        return;
    }
    model_->leaveRoom();
    model_->setStatusText(QStringLiteral("已离开房间"));
    registerMedia();  // 以空 roomId 重新注册，避免继续收到旧房间媒体
    refreshRooms();
}

void AppController::onRoomList(const QJsonArray& rooms) {
    QVector<RoomItem> list;
    list.reserve(rooms.size());
    for (const QJsonValue& value : rooms) {
        const QJsonObject obj = value.toObject();
        RoomItem item;
        item.roomId = obj.value("roomId").toString();
        item.roomName = obj.value("roomName").toString();
        item.ownerId = obj.value("ownerId").toString();
        item.memberCount = obj.value("memberCount").toInt();
        list.append(item);
    }
    model_->setRooms(list);
}

// ============================ 聊天 ============================

void AppController::sendChat(const QString& text) {
    const QString content = text.trimmed();
    if (content.isEmpty()) return;
    if (!model_->isLoggedIn()) {
        emit errorOccurred(QStringLiteral("请先登录"));
        return;
    }
    if (!model_->inRoom()) {
        emit errorOccurred(QStringLiteral("请先加入一个房间"));
        return;
    }

    signaling_->sendRoomChat(model_->currentRoomId(), content);

    // 服务端只广播给房间内其他人，本地需要自行回显
    ChatMessage msg;
    msg.userId = model_->selfUserId();
    msg.userName = model_->selfUserName();
    msg.content = content;
    msg.timestamp = QDateTime::currentSecsSinceEpoch();
    msg.self = true;
    model_->appendChat(msg);
}

void AppController::onChatReceived(const QString& userId, const QString& userName,
                                   const QString& roomId, const QString& content,
                                   qint64 timestamp) {
    ChatMessage msg;
    msg.userId = userId;
    msg.userName = userName;
    msg.content = content;
    msg.timestamp = timestamp;
    msg.self = (userId == model_->selfUserId());
    model_->appendChat(msg);
}

// ============================ 媒体 ============================

void AppController::toggleVideo(bool on) {
    if (on) {
        if (!model_->isLoggedIn()) {
            emit errorOccurred(QStringLiteral("请先登录后再开启摄像头"));
            return;
        }
        if (!media_->startVideoSend()) {
            emit errorOccurred(QStringLiteral("开启摄像头失败"));
        }
    } else {
        media_->stopVideoSend();
    }
}

void AppController::toggleMic(bool on) {
    if (on) {
        if (!model_->isLoggedIn()) {
            emit errorOccurred(QStringLiteral("请先登录后再开启麦克风"));
            return;
        }
        if (!media_->startAudioSend()) {
            emit errorOccurred(QStringLiteral("开启麦克风失败"));
        }
    } else {
        media_->stopAudioSend();
    }
}

void AppController::toggleSpeakerMuted(bool muted) {
    media_->setSpeakerMuted(muted);
    model_->setSpeakerMuted(muted);
}

void AppController::onVideoSendStateChanged(bool on) { model_->setVideoSendEnabled(on); }

void AppController::onAudioSendStateChanged(bool on) { model_->setMicEnabled(on); }

void AppController::onMediaStats(double localFps, int localKbps, double remoteFps, int remoteKbps) {
    model_->updateLocalStats(localFps, localKbps);
    model_->updateRemoteStats(remoteFps, remoteKbps);
}

void AppController::onMediaWarning(const QString& message) {
    // 媒体告警只做状态提示，避免启动阶段（如无麦克风）频繁弹窗打断用户
    model_->setStatusText(message);
}

void AppController::onMediaRegistered(bool success, const QString& message) {
    if (!success) {
        emit errorOccurred(QStringLiteral("媒体注册失败：%1").arg(message));
        return;
    }
    model_->setStatusText(
        QStringLiteral("媒体已注册：UDP %1（房间 %2）")
            .arg(media_->localPort())
            .arg(model_->currentRoomId().isEmpty() ? QStringLiteral("无")
                                                   : model_->currentRoomId()));
}

// ============================ 内部 ============================

void AppController::registerMedia() {
    if (!model_->isLoggedIn()) return;
    if (media_->localPort() == 0) return;

    const QString ip = signaling_->localAddress();
    if (ip.isEmpty()) return;

    media_->setRegisterInfo(ip, model_->currentRoomId());
    signaling_->sendMediaRegister(model_->currentRoomId(), ip, media_->localPort());
}

void AppController::onServerMessage(const QString& type, const QString& content) {
    if (type == QStringLiteral("ERROR")) {
        emit errorOccurred(QStringLiteral("服务端错误：%1").arg(content));
        model_->setStatusText(QStringLiteral("服务端错误：%1").arg(content));
    } else {
        model_->setStatusText(QStringLiteral("%1：%2").arg(type, content));
    }
}

void AppController::onSignalingError(const QString& message) {
    model_->setStatusText(message);
    emit errorOccurred(message);
}
