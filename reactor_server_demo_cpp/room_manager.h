#pragma once
// room_manager.h - 用户/房间/成员管理
#include <cstdint>
#include <functional>
#include <map>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

// 房间信息（对外展示用）
struct RoomInfo {
    std::string roomId;
    std::string roomName;
    std::string ownerId;
    std::vector<std::pair<std::string, std::string>> members;  // (userId, userName)
    size_t memberCount = 0;
};

// 房间内广播回调：先收集目标 fd 列表，释放锁后再回调发送
using RoomSender = std::function<void(int fd, const std::string& data)>;

class RoomManager {
public:
    // ---- 用户 ----
    // 登录/注册登录：fd <-> userId/userName 三张表登记；同一 userId 不允许重复登录
    bool login(int fd, const std::string& userId, const std::string& userName, std::string& err);
    // 登出/断开：移除用户映射并退出所有房间
    void logout(int fd);
    // fd 对应的 userId，未登录返回 ""
    std::string userIdOf(int fd) const;
    // fd 对应的 userName，未登录返回 ""
    std::string userNameOf(int fd) const;
    // 刷新 fd 最近活跃时间（收到任意信令时调用，心跳保活依据）
    void touch(int fd);

    // ---- 房间 ----
    bool createRoom(const std::string& roomId, const std::string& roomName, int ownerFd, std::string& err);
    bool joinRoom(const std::string& roomId, int fd, std::string& err);
    bool leaveRoom(const std::string& roomId, int fd, std::string& err);
    bool getRoomInfo(const std::string& roomId, RoomInfo& out, std::string& err) const;
    std::vector<RoomInfo> listRooms() const;
    // 查询房间当前成员 fd 列表（UDP 转发 fan-out 使用）
    std::vector<int> roomMemberFds(const std::string& roomId) const;

    // 房间广播：锁内收集目标 fd，锁外回调发送，避免持锁做 IO
    void broadcastToRoom(const std::string& roomId, int exceptFd,
                         const std::string& data, const RoomSender& sender);

    // ---- 心跳 ----
    size_t onlineCount() const;
    // 返回超过 timeoutSec 未收到任何信令的 fd 列表（由调用方负责关闭会话）
    std::vector<int> sweepIdle(int timeoutSec) const;

private:
    struct Room {
        std::string roomId;
        std::string roomName;
        std::string ownerId;
        std::map<int, std::string> members;  // fd -> userId
    };

    mutable std::mutex mtx_;
    std::map<int, std::string> fd_to_uid_;    // fd -> user
    std::map<int, std::string> fd_to_name_;   // fd -> userName
    std::map<std::string, int> uid_to_fd_;    // uid -> fd
    std::map<std::string, Room> rooms_;       // roomId -> room（room 内含 members）
    std::map<int, int64_t> last_active_;      // fd -> 最近活跃时间（秒）
};
