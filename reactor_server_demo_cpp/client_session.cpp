// client_session.cpp - 单个 TCP 客户端会话实现
#include "client_session.h"

#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>

#include "event_loop.h"
#include "logger.h"
#include "message_dispatcher.h"
#include "protocol_message.h"
#include "room_manager.h"
#include "udp_relay.h"
#include "utils.h"

ClientSession::ClientSession(int fd, EventLoop* loop, MessageDispatcher* dispatcher,
                             RoomManager* rooms, UdpRelay* udp)
    : fd_(fd), loop_(loop), dispatcher_(dispatcher), rooms_(rooms), udp_(udp) {
    setNonBlocking(fd_);
    int on = 1;
    ::setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));

    sockaddr_in addr{};
    socklen_t len = sizeof(addr);
    if (::getpeername(fd_, reinterpret_cast<sockaddr*>(&addr), &len) == 0) {
        peer_ = addrKey(addr);
    } else {
        peer_ = "unknown";
    }
    LOG_INFO("session created fd=%d peer=%s", fd_, peer_.c_str());
}

ClientSession::~ClientSession() {
    // 正常路径都走 closeSession()；此处兜底，防止对象被直接析构时泄漏 fd/状态
    if (!closed_) {
        udp_->unregisterFd(fd_);
        rooms_->logout(fd_);
        ::close(fd_);
    }
}

uint32_t ClientSession::events() const {
    uint32_t ev = EPOLLIN | EPOLLRDHUP;
    if (!write_buffer_.empty()) ev |= EPOLLOUT;
    return ev;
}

void ClientSession::handleEvent(uint32_t ev) {
    if (closed_) return;

    if (ev & (EPOLLHUP | EPOLLERR)) {
        handleEof("hup/err");
        return;
    }
    if (ev & EPOLLIN) onReadable();
    if (closed_) return;
    if (ev & EPOLLOUT) onWritable();
    // EPOLLRDHUP：对端关闭写端，读完剩余数据后关闭
    if (ev & EPOLLRDHUP) {
        onReadable();
        if (!closed_) handleEof("peer closed");
    }
}

void ClientSession::onReadable() {
    char buf[4096];
    while (true) {
        ssize_t n = ::recv(fd_, buf, sizeof(buf), 0);
        if (n > 0) {
            read_buffer_.append(buf, static_cast<size_t>(n));
            if (read_buffer_.size() > kMaxBufferSize) {
                handleEof("read buffer overflow");
                return;
            }
            continue;
        }
        if (n == 0) {  // 对端正常关闭
            handleEof("peer closed");
            return;
        }
        if (errno == EAGAIN || errno == EWOULDBLOCK) break;
        if (errno == EINTR) continue;
        handleEof(::strerror(errno));
        return;
    }
    processBuffer();
}

void ClientSession::processBuffer() {
    // TCP 粘包/半包处理：循环按 \n 拆帧，半包保留在 read_buffer_ 等待下次数据
    size_t searchFrom = 0;
    while (true) {
        size_t pos = read_buffer_.find('\n', searchFrom);
        if (pos == std::string::npos) break;

        std::string line = read_buffer_.substr(0, pos);
        if (!line.empty() && line.back() == '\r') line.pop_back();  // 兼容 telnet 的 \r\n
        read_buffer_.erase(0, pos + 1);
        searchFrom = 0;

        if (line.empty()) continue;

        ProtocolMessage msg;
        std::string err;
        if (!ProtocolMessage::fromJson(line, msg, err)) {
            ProtocolMessage reply = ProtocolMessage::makeReply(
                "ERROR", msg, false, "bad json: " + err);
            sendLine(reply.toJson());
            continue;
        }

        rooms_->touch(fd_);  // 任意信令都刷新心跳
        dispatcher_->dispatch(fd_, msg);
        if (closed_) return;
    }
}

void ClientSession::sendLine(const std::string& line) {
    if (closed_) return;
    write_buffer_.append(line);
    write_buffer_.push_back('\n');
    flushWrite();
}

void ClientSession::flushWrite() {
    while (!write_buffer_.empty()) {
        ssize_t n = ::send(fd_, write_buffer_.data(), write_buffer_.size(), MSG_NOSIGNAL);
        if (n > 0) {
            write_buffer_.erase(0, static_cast<size_t>(n));
            continue;
        }
        if (n < 0 && errno == EINTR) continue;
        if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) break;
        handleEof(n < 0 ? ::strerror(errno) : "send failed");
        return;
    }
    updateEpollEvents();  // 根据剩余写缓冲决定是否关注 EPOLLOUT
}

void ClientSession::onWritable() {
    flushWrite();
}

void ClientSession::updateEpollEvents() {
    loop_->modHandler(fd_, events());
}

void ClientSession::handleEof(const char* reason) {
    LOG_INFO("session eof fd=%d peer=%s reason=%s", fd_, peer_.c_str(), reason);
    closeSession();
}

void ClientSession::closeSession() {
    if (closed_) return;
    closed_ = true;

    rooms_->logout(fd_);       // 退出房间、移除用户映射
    udp_->unregisterFd(fd_);   // 注销媒体地址
    loop_->delHandler(fd_);    // 从 EventLoop 移除（引用计数归零后自动析构）
    ::close(fd_);
    LOG_INFO("session closed fd=%d peer=%s", fd_, peer_.c_str());
}
