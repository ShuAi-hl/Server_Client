#pragma once
// udp_relay.h - UDP / RTP 媒体中继
// UDP 端口接收音视频数据报，按房间成员表 fan-out 转发（不回发给发送者）
#include <sys/epoll.h>
#include <map>
#include <mutex>
#include <string>

#include "ievent_handler.h"
#include "utils.h"

class RoomManager;

class UdpRelay : public IEventHandler {
public:
    // 绑定 UDP 端口并注册到事件循环；bind 失败时 ok() 为 false
    UdpRelay(class EventLoop* loop, uint16_t port, RoomManager* rooms);
    ~UdpRelay() override;

    UdpRelay(const UdpRelay&) = delete;
    UdpRelay& operator=(const UdpRelay&) = delete;

    bool ok() const { return sock_fd_.get() >= 0; }
    uint16_t port() const { return port_; }

    // MEDIA_REGISTER：登记客户端 UDP 媒体地址（fd -> 地址，地址 -> fd 反查表）
    void registerMedia(int fd, const std::string& roomId,
                       const std::string& ip, uint16_t port);
    // 会话关闭时注销媒体地址
    void unregisterFd(int fd);

    // ---- IEventHandler ----
    int fd() const override { return sock_fd_.get(); }
    uint32_t events() const override { return EPOLLIN; }
    void handleEvent(uint32_t events) override;

private:
    struct MediaReg {
        std::string roomId;
        sockaddr_in addr{};
    };

    void onReadable();

    EventLoop* loop_;
    FdGuard sock_fd_;
    uint16_t port_;
    RoomManager* rooms_;

    std::mutex mtx_;
    std::map<int, MediaReg> fd_to_media_;    // fd -> 媒体注册信息
    std::map<std::string, int> addr_to_fd_;  // "ip:port" -> fd（收到数据报时反查）
};
