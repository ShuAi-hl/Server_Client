#pragma once
// event_loop.h - epoll 事件循环核心（Reactor）
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <vector>

#include "ievent_handler.h"

class EventLoop {
public:
    EventLoop();
    ~EventLoop();

    EventLoop(const EventLoop&) = delete;
    EventLoop& operator=(const EventLoop&) = delete;

    // 注册处理器（epoll ADD + map 持有 shared_ptr，避免对象过早释放）
    void addHandler(const std::shared_ptr<IEventHandler>& handler);
    // 注册非托管处理器（如栈上创建的 Acceptor/UdpRelay，生命周期由调用方保证）
    void addHandler(IEventHandler* rawHandler);
    // 修改关注事件（fd 对应处理器已注册时使用）
    void modHandler(int fd, uint32_t events);
    // 注销处理器（epoll DEL + 从 map 移除，引用计数归零后自动销毁）
    void delHandler(int fd);

    // 取出处理器（供控制台/清扫任务使用）
    std::shared_ptr<IEventHandler> getHandler(int fd) const;

    // 跨线程投递任务到事件循环线程执行（控制台命令、心跳清扫均经此进入）
    void runInLoop(std::function<void()> task);

    void quit() { quit_ = true; }

    // 主循环：epoll_wait + 事件分发 + 执行投递任务
    void loop();

private:
    void runPendingTasks();

    int epoll_fd_ = -1;
    std::map<int, std::shared_ptr<IEventHandler>> handlers_;
    std::mutex task_mtx_;
    std::vector<std::function<void()>> tasks_;
    bool quit_ = false;
};
