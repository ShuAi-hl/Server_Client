// acceptor.cpp - TCP 监听连接器实现
#include "acceptor.h"

#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cerrno>
#include <cstring>

#include "client_session.h"
#include "event_loop.h"
#include "logger.h"

Acceptor::Acceptor(EventLoop* loop, uint16_t port, MessageDispatcher* dispatcher,
                   RoomManager* rooms, UdpRelay* udp, std::string& err)
    : loop_(loop), port_(port), dispatcher_(dispatcher), rooms_(rooms), udp_(udp) {
    int fd = ::createTcpListener(port);
    if (fd < 0) {
        err = std::string("listen on port ") + std::to_string(port) + " failed: " +
              ::strerror(errno);
        return;
    }
    listen_fd_.reset(fd);
    loop_->addHandler(this);
    LOG_INFO("tcp server listening on port %u", port);
}

Acceptor::~Acceptor() {
    if (loop_ && listen_fd_.get() >= 0) {
        loop_->delHandler(listen_fd_.get());
    }
}

void Acceptor::handleEvent(uint32_t events) {
    if (events & EPOLLIN) onAccept();
}

void Acceptor::onAccept() {
    // 工厂模式：接收连接后创建 ClientSession 并注册到 loop
    while (true) {
        sockaddr_in peer{};
        socklen_t peerlen = sizeof(peer);
        int cfd = ::accept(listen_fd_.get(), reinterpret_cast<sockaddr*>(&peer), &peerlen);
        if (cfd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) break;
            if (errno == EINTR) continue;
            LOG_ERROR("accept failed: %s", ::strerror(errno));
            break;
        }

        auto session = std::make_shared<ClientSession>(cfd, loop_, dispatcher_, rooms_, udp_);
        loop_->addHandler(session);
    }
}
