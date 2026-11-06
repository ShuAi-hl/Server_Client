#pragma once
// utils.h - socket 工具：socket 创建、非阻塞、RAII FdGuard
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <string>

// RAII fd 守卫：析构自动 close()，支持移动语义，禁止拷贝
class FdGuard {
public:
    FdGuard() = default;
    explicit FdGuard(int fd) : fd_(fd) {}
    ~FdGuard() { reset(); }

    FdGuard(const FdGuard&) = delete;
    FdGuard& operator=(const FdGuard&) = delete;

    FdGuard(FdGuard&& other) noexcept : fd_(other.fd_) { other.fd_ = -1; }
    FdGuard& operator=(FdGuard&& other) noexcept {
        if (this != &other) {
            reset();
            fd_ = other.fd_;
            other.fd_ = -1;
        }
        return *this;
    }

    int get() const { return fd_; }
    int release() {
        int fd = fd_;
        fd_ = -1;
        return fd;
    }
    void reset(int fd = -1) {
        if (fd_ >= 0) ::close(fd_);
        fd_ = fd;
    }
    explicit operator bool() const { return fd_ >= 0; }

private:
    int fd_ = -1;
};

// 设置 fd 为非阻塞
inline int setNonBlocking(int fd) {
    int flags = ::fcntl(fd, F_GETFL, 0);
    if (flags < 0) return -1;
    return ::fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

// 创建 TCP 监听 socket（已 bind + listen + 非阻塞），失败返回 -1
inline int createTcpListener(uint16_t port) {
    int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;

    int on = 1;
    ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    if (::bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        ::close(fd);
        return -1;
    }
    if (::listen(fd, 128) < 0) {
        ::close(fd);
        return -1;
    }
    setNonBlocking(fd);
    return fd;
}

// 创建 UDP socket（已 bind + 非阻塞），失败返回 -1
inline int createUdpSocket(uint16_t port) {
    int fd = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) return -1;

    int on = 1;
    ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    if (::bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        ::close(fd);
        return -1;
    }
    setNonBlocking(fd);
    return fd;
}

// sockaddr_in -> "ip:port" 字符串，用作媒体地址表 key
inline std::string addrKey(const sockaddr_in& addr) {
    char buf[64];
    ::snprintf(buf, sizeof(buf), "%s:%u",
               ::inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
    return buf;
}
