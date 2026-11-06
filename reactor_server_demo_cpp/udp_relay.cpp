// udp_relay.cpp - UDP / RTP 媒体中继实现
#include "udp_relay.h"

#include <sys/epoll.h>
#include <cstring>
#include <cerrno>

#include "event_loop.h"
#include "logger.h"
#include "room_manager.h"

UdpRelay::UdpRelay(EventLoop* loop, uint16_t port, RoomManager* rooms)
    : loop_(loop), port_(port), rooms_(rooms) {
    int fd = ::createUdpSocket(port);
    if (fd < 0) {
        LOG_ERROR("udp relay bind port %u failed: %s", port, ::strerror(errno));
        return;
    }
    sock_fd_.reset(fd);
    loop_->addHandler(this);
    LOG_INFO("udp relay listening on port %u", port);
}

UdpRelay::~UdpRelay() {
    if (loop_ && sock_fd_.get() >= 0) {
        loop_->delHandler(sock_fd_.get());
    }
}

void UdpRelay::registerMedia(int fd, const std::string& roomId,
                             const std::string& ip, uint16_t port) {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (::inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) != 1) {
        LOG_WARN("registerMedia fd=%d invalid ip '%s'", fd, ip.c_str());
        return;
    }

    std::string key = addrKey(addr);
    std::lock_guard<std::mutex> lock(mtx_);
    // 同一 fd 重复注册：先清理旧地址映射
    auto fit = fd_to_media_.find(fd);
    if (fit != fd_to_media_.end()) {
        addr_to_fd_.erase(addrKey(fit->second.addr));
    }
    // 同一地址被其他 fd 注册：清理旧 fd 映射
    auto ait = addr_to_fd_.find(key);
    if (ait != addr_to_fd_.end() && ait->second != fd) {
        fd_to_media_.erase(ait->second);
    }
    MediaReg reg;
    reg.roomId = roomId;
    reg.addr = addr;
    fd_to_media_[fd] = std::move(reg);
    addr_to_fd_[key] = fd;
    LOG_INFO("media registered fd=%d room=%s addr=%s", fd, roomId.c_str(), key.c_str());
}

void UdpRelay::unregisterFd(int fd) {
    std::lock_guard<std::mutex> lock(mtx_);
    auto fit = fd_to_media_.find(fd);
    if (fit == fd_to_media_.end()) return;
    addr_to_fd_.erase(addrKey(fit->second.addr));
    fd_to_media_.erase(fit);
    LOG_INFO("media unregistered fd=%d", fd);
}

void UdpRelay::handleEvent(uint32_t events) {
    if (events & EPOLLIN) onReadable();
}

void UdpRelay::onReadable() {
    char buf[2048];
    while (true) {
        sockaddr_in peer{};
        socklen_t peerlen = sizeof(peer);
        ssize_t n = ::recvfrom(sock_fd_.get(), buf, sizeof(buf), 0,
                               reinterpret_cast<sockaddr*>(&peer), &peerlen);
        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) break;
            if (errno == EINTR) continue;
            LOG_ERROR("udp recvfrom failed: %s", ::strerror(errno));
            break;
        }

        // 反查发送者 -> fd -> 所在房间
        int senderFd = -1;
        {
            std::lock_guard<std::mutex> lock(mtx_);
            auto it = addr_to_fd_.find(addrKey(peer));
            if (it == addr_to_fd_.end()) continue;  // 未注册的发送者：忽略
            senderFd = it->second;
        }

        std::string roomId;
        {
            std::lock_guard<std::mutex> lock(mtx_);
            auto it = fd_to_media_.find(senderFd);
            if (it == fd_to_media_.end()) continue;
            roomId = it->second.roomId;
        }

        // 按房间成员表 fan-out 转发给其他成员（不回发给发送者）
        std::vector<int> members = rooms_->roomMemberFds(roomId);
        size_t forwarded = 0;
        {
            std::lock_guard<std::mutex> lock(mtx_);
            for (int fd : members) {
                if (fd == senderFd) continue;
                auto it = fd_to_media_.find(fd);
                if (it == fd_to_media_.end()) continue;  // 成员未注册媒体地址
                ssize_t sent = ::sendto(sock_fd_.get(), buf, static_cast<size_t>(n), 0,
                                        reinterpret_cast<const sockaddr*>(&it->second.addr),
                                        sizeof(it->second.addr));
                if (sent >= 0) ++forwarded;
            }
        }
        if (forwarded > 0) {
            LOG_DEBUG("udp relay room=%s from fd=%d forwarded %zu packet(s) (%zd bytes)",
                      roomId.c_str(), senderFd, forwarded, n);
        }
    }
}
