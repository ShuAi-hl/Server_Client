#pragma once
// AppModel.h - 应用数据模型（MVC 中的 M）
// 只保存状态并对变化发出信号，不持有任何界面对象，也不主动发起网络请求。
#include <QList>
#include <QObject>
#include <QString>
#include <QVector>

// 房间列表条目
struct RoomItem {
    QString roomId;
    QString roomName;
    QString ownerId;
    int memberCount = 0;
};

// 房间成员
struct MemberItem {
    QString userId;
    QString userName;
};

// 聊天消息
struct ChatMessage {
    QString userId;
    QString userName;
    QString content;
    qint64 timestamp = 0;
    bool self = false;
};

class AppModel : public QObject {
    Q_OBJECT
public:
    // 连接状态机
    enum class ConnState {
        Disconnected = 0,
        Connecting,
        Connected,
        LoggedIn,
    };

    explicit AppModel(QObject* parent = nullptr);

    // ---------------- 连接与用户 ----------------
    ConnState connState() const { return connState_; }
    bool isLoggedIn() const { return connState_ == ConnState::LoggedIn; }
    void setConnState(ConnState state);

    void setServerInfo(const QString& host, quint16 port);
    QString serverHost() const { return serverHost_; }
    quint16 serverPort() const { return serverPort_; }

    void setSelfUser(const QString& userId, const QString& userName);
    QString selfUserId() const { return selfUserId_; }
    QString selfUserName() const { return selfUserName_; }

    // ---------------- 房间 ----------------
    const QVector<RoomItem>& rooms() const { return rooms_; }
    void setRooms(const QVector<RoomItem>& rooms);

    QString currentRoomId() const { return currentRoomId_; }
    QString currentRoomName() const { return currentRoomName_; }
    QString currentRoomOwnerId() const { return currentRoomOwnerId_; }
    bool inRoom() const { return !currentRoomId_.isEmpty(); }
    void enterRoom(const QString& roomId, const QString& roomName, const QString& ownerId);
    void leaveRoom();

    const QVector<MemberItem>& members() const { return members_; }
    void setMembers(const QVector<MemberItem>& members);

    // ---------------- 聊天 ----------------
    const QList<ChatMessage>& chatMessages() const { return chats_; }
    void appendChat(const ChatMessage& msg);
    void clearChat();

    // ---------------- 媒体状态 ----------------
    void setMediaPort(quint16 port);
    quint16 mediaPort() const { return mediaPort_; }

    void setVideoSendEnabled(bool on);
    bool videoSendEnabled() const { return videoSendEnabled_; }

    void setMicEnabled(bool on);
    bool micEnabled() const { return micEnabled_; }

    void setSpeakerMuted(bool muted);
    bool speakerMuted() const { return speakerMuted_; }

    void setAudioDevices(bool hasInput, bool hasOutput);
    bool hasAudioInput() const { return hasAudioInput_; }
    bool hasAudioOutput() const { return hasAudioOutput_; }

    // 媒体统计（每秒刷新一次）
    void updateLocalStats(double fps, int kbps);
    void updateRemoteStats(double fps, int kbps);
    double localFps() const { return localFps_; }
    int localKbps() const { return localKbps_; }
    double remoteFps() const { return remoteFps_; }
    int remoteKbps() const { return remoteKbps_; }

    // ---------------- 状态栏提示 ----------------
    QString statusText() const { return statusText_; }
    void setStatusText(const QString& text);

signals:
    void connStateChanged(AppModel::ConnState state);
    void serverInfoChanged();
    void selfUserChanged();
    void roomsChanged();
    void currentRoomChanged();
    void membersChanged();
    void chatChanged();
    void mediaStateChanged();
    void mediaStatsChanged();
    void statusTextChanged(const QString& text);

private:
    ConnState connState_ = ConnState::Disconnected;

    QString serverHost_;
    quint16 serverPort_ = 8888;

    QString selfUserId_;
    QString selfUserName_;

    QVector<RoomItem> rooms_;
    QString currentRoomId_;
    QString currentRoomName_;
    QString currentRoomOwnerId_;
    QVector<MemberItem> members_;

    QList<ChatMessage> chats_;

    quint16 mediaPort_ = 0;
    bool videoSendEnabled_ = false;
    bool micEnabled_ = false;
    bool speakerMuted_ = false;
    bool hasAudioInput_ = false;
    bool hasAudioOutput_ = false;

    double localFps_ = 0.0;
    int localKbps_ = 0;
    double remoteFps_ = 0.0;
    int remoteKbps_ = 0;

    QString statusText_;
};
