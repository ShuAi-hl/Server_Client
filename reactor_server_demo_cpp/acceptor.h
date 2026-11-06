#pragma once
// acceptor.h - TCP 监听连接器（工厂角色：接收连接后创建 ClientSession 并注册到 loop）
#include <sys/epoll.h>
#include <string>

#include "ievent_handler.h"
#include "utils.h"

class EventLoop;
class MessageDispatcher;
class RoomManager;
class UdpRelay;

class Acceptor : public IEventHandler {
public:
    // 创建监听 socket 并注册到事件循环；失败时 ok() 为 false，err 输出原因
    Acceptor(EventLoop* loop, uint16_t port, MessageDispatcher* dispatcher,
             RoomManager* rooms, UdpRelay* udp, std::string& err);
    ~Acceptor() override;

    Acceptor(const Acceptor&) = delete;
    Acceptor& operator=(const Acceptor&) = delete;

    bool ok() const { return listen_fd_.get() >= 0; }
    uint16_t port() const { return port_; }

    // ---- IEventHandler ----
    int fd() const override { return listen_fd_.get(); }
    uint32_t events() const override { return EPOLLIN; }
    void handleEvent(uint32_t events) override;

private:
    void onAccept();

    EventLoop* loop_;
    FdGuard listen_fd_;
    uint16_t port_;
    MessageDispatcher* dispatcher_;
    RoomManager* rooms_;
    UdpRelay* udp_;
};
