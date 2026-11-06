#pragma once
// message_dispatcher.h - map + lambda 注册式消息分发器
#include <functional>
#include <map>
#include <string>

#include "protocol_message.h"

// 消息处理函数签名：连接 fd + 协议消息
using MessageHandler = std::function<void(int fd, const ProtocolMessage& msg)>;

class MessageDispatcher {
public:
    // 注册某消息类型的处理函数；启动时以 lambda 注册，新增业务无需改动框架
    void registerHandler(const std::string& type, MessageHandler handler) {
        handlers_[type] = std::move(handler);
    }

    // 未注册消息类型的兜底处理（如回复 ERROR）
    void setFallbackHandler(MessageHandler handler) {
        fallback_ = std::move(handler);
    }

    // 分发一条消息到对应处理函数
    void dispatch(int fd, const ProtocolMessage& msg) {
        auto it = handlers_.find(msg.type);
        if (it != handlers_.end()) {
            it->second(fd, msg);
        } else if (fallback_) {
            fallback_(fd, msg);
        }
    }

private:
    std::map<std::string, MessageHandler> handlers_;
    MessageHandler fallback_;
};
