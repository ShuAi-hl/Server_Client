// room_manager.cpp - 用户/房间/成员管理实现
#include "room_manager.h"

#include <ctime>

#include "logger.h"

bool RoomManager::login(int fd, const std::string& userId, const std::string& userName, std::string& err) {
    if (userId.empty()) {
        err = "userId is required";
        return false;
    }
    std::lock_guard<std::mutex> lock(mtx_);

    // 同一 userId 已在其他连接登录：拒绝，保持先登录者
    auto uit = uid_to_fd_.find(userId);
    if (uit != uid_to_fd_.end() && uit->second != fd) {
        err = "userId already online";
        return false;
    }

    // 重复登录：先清理旧映射
    auto fit = fd_to_uid_.find(fd);
    if (fit != fd_to_uid_.end() && fit->second != userId) {
        uid_to_fd_.erase(fit->second);
    }

    fd_to_uid_[fd] = userId;
    fd_to_name_[fd] = userName.empty() ? userId : userName;
    uid_to_fd_[userId] = fd;
    last_active_[fd] = ::time(nullptr);
    return true;
}

void RoomManager::logout(int fd) {
    std::lock_guard<std::mutex> lock(mtx_);

    auto fit = fd_to_uid_.find(fd);
    if (fit == fd_to_uid_.end()) return;

    std::string uid = fit->second;
    fd_to_uid_.erase(fit);
    fd_to_name_.erase(fd);
    uid_to_fd_.erase(uid);
    last_active_.erase(fd);

    // 退出所有房间
    for (auto& kv : rooms_) {
        kv.second.members.erase(fd);
    }
    LOG_INFO("user %s logged out (fd=%d)", uid.c_str(), fd);
}

std::string RoomManager::userIdOf(int fd) const {
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = fd_to_uid_.find(fd);
    return it == fd_to_uid_.end() ? "" : it->second;
}

std::string RoomManager::userNameOf(int fd) const {
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = fd_to_name_.find(fd);
    return it == fd_to_name_.end() ? "" : it->second;
}

void RoomManager::touch(int fd) {
    std::lock_guard<std::mutex> lock(mtx_);
    last_active_[fd] = ::time(nullptr);
}

bool RoomManager::createRoom(const std::string& roomId, const std::string& roomName,
                             int ownerFd, std::string& err) {
    if (roomId.empty()) {
        err = "roomId is required";
        return false;
    }
    std::lock_guard<std::mutex> lock(mtx_);

    auto uit = fd_to_uid_.find(ownerFd);
    if (uit == fd_to_uid_.end()) {
        err = "not logged in";
        return false;
    }
    if (rooms_.count(roomId)) {
        err = "room already exists";
        return false;
    }

    Room room;
    room.roomId = roomId;
    room.roomName = roomName.empty() ? roomId : roomName;
    room.ownerId = uit->second;
    room.members[ownerFd] = uit->second;  // 创建者自动加入房间
    rooms_[roomId] = std::move(room);
    last_active_[ownerFd] = ::time(nullptr);
    return true;
}

bool RoomManager::joinRoom(const std::string& roomId, int fd, std::string& err) {
    std::lock_guard<std::mutex> lock(mtx_);

    auto uit = fd_to_uid_.find(fd);
    if (uit == fd_to_uid_.end()) {
        err = "not logged in";
        return false;
    }
    auto rit = rooms_.find(roomId);
    if (rit == rooms_.end()) {
        err = "room not found";
        return false;
    }
    rit->second.members[fd] = uit->second;
    last_active_[fd] = ::time(nullptr);
    return true;
}

bool RoomManager::leaveRoom(const std::string& roomId, int fd, std::string& err) {
    std::lock_guard<std::mutex> lock(mtx_);

    auto rit = rooms_.find(roomId);
    if (rit == rooms_.end()) {
        err = "room not found";
        return false;
    }
    if (rit->second.members.erase(fd) == 0) {
        err = "not in room";
        return false;
    }
    last_active_[fd] = ::time(nullptr);
    return true;
}

bool RoomManager::getRoomInfo(const std::string& roomId, RoomInfo& out, std::string& err) const {
    std::lock_guard<std::mutex> lock(mtx_);
    auto rit = rooms_.find(roomId);
    if (rit == rooms_.end()) {
        err = "room not found";
        return false;
    }
    const Room& room = rit->second;
    out.roomId = room.roomId;
    out.roomName = room.roomName;
    out.ownerId = room.ownerId;
    out.memberCount = room.members.size();
    out.members.clear();
    for (const auto& kv : room.members) {
        auto nit = fd_to_name_.find(kv.first);
        std::string name = nit != fd_to_name_.end() ? nit->second : kv.second;
        out.members.emplace_back(kv.second, name);
    }
    return true;
}

std::vector<RoomInfo> RoomManager::listRooms() const {
    std::lock_guard<std::mutex> lock(mtx_);
    std::vector<RoomInfo> list;
    list.reserve(rooms_.size());
    for (const auto& kv : rooms_) {
        const Room& room = kv.second;
        RoomInfo info;
        info.roomId = room.roomId;
        info.roomName = room.roomName;
        info.ownerId = room.ownerId;
        info.memberCount = room.members.size();
        for (const auto& kv2 : room.members) {
            auto nit = fd_to_name_.find(kv2.first);
            std::string name = nit != fd_to_name_.end() ? nit->second : kv2.second;
            info.members.emplace_back(kv2.second, name);
        }
        list.push_back(std::move(info));
    }
    return list;
}

std::vector<int> RoomManager::roomMemberFds(const std::string& roomId) const {
    std::lock_guard<std::mutex> lock(mtx_);
    std::vector<int> fds;
    auto rit = rooms_.find(roomId);
    if (rit != rooms_.end()) {
        fds.reserve(rit->second.members.size());
        for (const auto& kv : rit->second.members) {
            fds.push_back(kv.first);
        }
    }
    return fds;
}

void RoomManager::broadcastToRoom(const std::string& roomId, int exceptFd,
                                  const std::string& data, const RoomSender& sender) {
    std::vector<int> targets;
    {
        // 锁内只收集目标 fd，不做 IO
        std::lock_guard<std::mutex> lock(mtx_);
        auto rit = rooms_.find(roomId);
        if (rit == rooms_.end()) return;
        targets.reserve(rit->second.members.size());
        for (const auto& kv : rit->second.members) {
            if (kv.first != exceptFd) targets.push_back(kv.first);
        }
    }
    // 锁外回调发送
    for (int fd : targets) {
        sender(fd, data);
    }
}

size_t RoomManager::onlineCount() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return fd_to_uid_.size();
}

std::vector<int> RoomManager::sweepIdle(int timeoutSec) const {
    int64_t now = ::time(nullptr);
    std::vector<int> idle;
    std::lock_guard<std::mutex> lock(mtx_);
    for (const auto& kv : last_active_) {
        if (now - kv.second > timeoutSec) {
            idle.push_back(kv.first);
        }
    }
    return idle;
}
