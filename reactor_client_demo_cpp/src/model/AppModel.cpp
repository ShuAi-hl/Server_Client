// AppModel.cpp - 应用数据模型实现
#include "AppModel.h"

AppModel::AppModel(QObject* parent) : QObject(parent) {}

void AppModel::setConnState(ConnState state) {
    if (connState_ == state) return;
    connState_ = state;
    emit connStateChanged(connState_);
}

void AppModel::setServerInfo(const QString& host, quint16 port) {
    if (serverHost_ == host && serverPort_ == port) return;
    serverHost_ = host;
    serverPort_ = port;
    emit serverInfoChanged();
}

void AppModel::setSelfUser(const QString& userId, const QString& userName) {
    if (selfUserId_ == userId && selfUserName_ == userName) return;
    selfUserId_ = userId;
    selfUserName_ = userName;
    emit selfUserChanged();
}

void AppModel::setRooms(const QVector<RoomItem>& rooms) {
    rooms_ = rooms;
    emit roomsChanged();
}

void AppModel::enterRoom(const QString& roomId, const QString& roomName, const QString& ownerId) {
    currentRoomId_ = roomId;
    currentRoomName_ = roomName.isEmpty() ? roomId : roomName;
    currentRoomOwnerId_ = ownerId;
    emit currentRoomChanged();
}

void AppModel::leaveRoom() {
    if (currentRoomId_.isEmpty() && members_.isEmpty()) return;
    currentRoomId_.clear();
    currentRoomName_.clear();
    currentRoomOwnerId_.clear();
    members_.clear();
    emit currentRoomChanged();
    emit membersChanged();
}

void AppModel::setMembers(const QVector<MemberItem>& members) {
    members_ = members;
    emit membersChanged();
}

void AppModel::appendChat(const ChatMessage& msg) {
    chats_.append(msg);
    // 只保留最近 300 条，避免长时间运行后界面变慢
    while (chats_.size() > 300) chats_.removeFirst();
    emit chatChanged();
}

void AppModel::clearChat() {
    if (chats_.isEmpty()) return;
    chats_.clear();
    emit chatChanged();
}

void AppModel::setMediaPort(quint16 port) {
    if (mediaPort_ == port) return;
    mediaPort_ = port;
    emit mediaStateChanged();
}

void AppModel::setVideoSendEnabled(bool on) {
    if (videoSendEnabled_ == on) return;
    videoSendEnabled_ = on;
    emit mediaStateChanged();
}

void AppModel::setMicEnabled(bool on) {
    if (micEnabled_ == on) return;
    micEnabled_ = on;
    emit mediaStateChanged();
}

void AppModel::setSpeakerMuted(bool muted) {
    if (speakerMuted_ == muted) return;
    speakerMuted_ = muted;
    emit mediaStateChanged();
}

void AppModel::setAudioDevices(bool hasInput, bool hasOutput) {
    if (hasAudioInput_ == hasInput && hasAudioOutput_ == hasOutput) return;
    hasAudioInput_ = hasInput;
    hasAudioOutput_ = hasOutput;
    emit mediaStateChanged();
}

void AppModel::updateLocalStats(double fps, int kbps) {
    if (qFuzzyCompare(localFps_ + 1.0, fps + 1.0) && localKbps_ == kbps) return;
    localFps_ = fps;
    localKbps_ = kbps;
    emit mediaStatsChanged();
}

void AppModel::updateRemoteStats(double fps, int kbps) {
    if (qFuzzyCompare(remoteFps_ + 1.0, fps + 1.0) && remoteKbps_ == kbps) return;
    remoteFps_ = fps;
    remoteKbps_ = kbps;
    emit mediaStatsChanged();
}

void AppModel::setStatusText(const QString& text) {
    if (statusText_ == text) return;
    statusText_ = text;
    emit statusTextChanged(statusText_);
}
