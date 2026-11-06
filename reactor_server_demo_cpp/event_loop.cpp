// event_loop.cpp - epoll 事件循环核心实现
#include "event_loop.h"
#include "logger.h"

#include <sys/epoll.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>

namespace {
constexpr int kMaxEvents = 1024;
constexpr int kTimeoutMs = 1000;  // 1s，保证投递任务及时执行
}

EventLoop::EventLoop() {
    epoll_fd_ = ::epoll_create1(EPOLL_CLOEXEC);
    if (epoll_fd_ < 0) {
        LOG_ERROR("epoll_create1 failed: %s", ::strerror(errno));
    }
}

EventLoop::~EventLoop() {
    if (epoll_fd_ >= 0) ::close(epoll_fd_);
}

void EventLoop::addHandler(const std::shared_ptr<IEventHandler>& handler) {
    if (epoll_fd_ < 0 || !handler) return;

    epoll_event ev{};
    ev.events = handler->events();
    ev.data.fd = handler->fd();
    if (::epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, handler->fd(), &ev) < 0) {
        LOG_ERROR("epoll ADD fd=%d failed: %s", handler->fd(), ::strerror(errno));
        return;
    }
    handlers_[handler->fd()] = handler;  // map 持有 shared_ptr
}

void EventLoop::addHandler(IEventHandler* rawHandler) {
    // 用空删除器包装成 shared_ptr 存入 map，统一管理但不接管生命周期
    if (!rawHandler) return;
    addHandler(std::shared_ptr<IEventHandler>(rawHandler, [](IEventHandler*) {}));
}

void EventLoop::modHandler(int fd, uint32_t events) {
    if (epoll_fd_ < 0) return;
    if (handlers_.find(fd) == handlers_.end()) return;

    epoll_event ev{};
    ev.events = events;
    ev.data.fd = fd;
    if (::epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &ev) < 0) {
        LOG_ERROR("epoll MOD fd=%d failed: %s", fd, ::strerror(errno));
    }
}

void EventLoop::delHandler(int fd) {
    if (epoll_fd_ >= 0) {
        ::epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, nullptr);  // 忽略错误（fd 可能已失效）
    }
    handlers_.erase(fd);  // shared_ptr 引用计数归零后自动析构
}

std::shared_ptr<IEventHandler> EventLoop::getHandler(int fd) const {
    auto it = handlers_.find(fd);
    return it == handlers_.end() ? nullptr : it->second;
}

void EventLoop::runInLoop(std::function<void()> task) {
    std::lock_guard<std::mutex> lock(task_mtx_);
    tasks_.push_back(std::move(task));
}

void EventLoop::runPendingTasks() {
    std::vector<std::function<void()>> tasks;
    {
        std::lock_guard<std::mutex> lock(task_mtx_);
        tasks.swap(tasks_);
    }
    for (auto& task : tasks) task();
}

void EventLoop::loop() {
    std::vector<epoll_event> events(kMaxEvents);

    while (!quit_) {
        int n = ::epoll_wait(epoll_fd_, events.data(), static_cast<int>(events.size()), kTimeoutMs);
        if (n < 0) {
            if (errno == EINTR) continue;
            LOG_ERROR("epoll_wait failed: %s", ::strerror(errno));
            break;
        }

        for (int i = 0; i < n; ++i) {
            // 取 shared_ptr 副本，保证 handleEvent 执行期间对象不被释放
            auto it = handlers_.find(events[i].data.fd);
            if (it == handlers_.end()) continue;
            auto handler = it->second;
            handler->handleEvent(events[i].events);
        }

        runPendingTasks();
    }

    LOG_INFO("event loop exited");
}
