#pragma once
// client_session.h - 单个 TCP 客户端会话
// 拆帧、JSON 反序列化、调用 dispatcher、发送响应；断开时自动清理用户/房间/媒体注册
#include <cstdint>
#include <memory>
#include <string>

#include "ievent_handler.h"

class EventLoop;
class MessageDispatcher;
class RoomManager;
class UdpRelay;

class ClientSession : public IEventHandler,
                      public std::enable_shared_from_this<ClientSession> {
public:
    ClientSession(int fd, EventLoop* loop, MessageDispatcher* dispatcher,
                  RoomManager* rooms, UdpRelay* udp);
    ~ClientSession() override;

    ClientSession(const ClientSession&) = delete;
    ClientSession& operator=(const ClientSession&) = delete;

    // 向客户端发送一行 JSON（自动追加 \n）
    void sendLine(const std::string& line);
    // 关闭会话：从 EventLoop 移除、关闭 fd、清理用户/房间/媒体注册
    void closeSession();

    const std::string& peer() const { return peer_; }

    // ---- IEventHandler ----
    int fd() const override { return fd_; }
    uint32_t events() const override;
    void handleEvent(uint32_t events) override;

private:
    void onReadable();
    void onWritable();
    void handleEof(const char* reason);
    // 按行拆帧：read_buffer_ 中循环查找 \n，半包保留到下次处理
    void processBuffer();
    // 尝试把 write_buffer_ 发送出去；未发完则关注 EPOLLOUT
    void flushWrite();
    void updateEpollEvents();

    int fd_;
    EventLoop* loop_;
    MessageDispatcher* dispatcher_;
    RoomManager* rooms_;
    UdpRelay* udp_;

    std::string peer_;
    std::string read_buffer_;
    std::string write_buffer_;
    bool closed_ = false;

    static constexpr size_t kMaxBufferSize = 256 * 1024;  // 防御异常超大包
};
