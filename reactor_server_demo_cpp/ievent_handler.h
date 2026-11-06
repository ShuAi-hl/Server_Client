#pragma once
// ievent_handler.h - Reactor 事件处理器抽象接口
#include <cstdint>

class IEventHandler {
public:
    virtual ~IEventHandler() = default;

    // 关心的 fd
    virtual int fd() const = 0;
    // 关心的 epoll 事件
    virtual uint32_t events() const = 0;
    // epoll 事件回调
    virtual void handleEvent(uint32_t events) = 0;
};
