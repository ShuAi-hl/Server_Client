// main.cpp - 服务入口：组装组件、注册消息处理器、控制台命令、心跳清扫
#include <signal.h>
#include <unistd.h>

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>
#include <thread>

#include "acceptor.h"
#include "client_session.h"
#include "event_loop.h"
#include "logger.h"
#include "message_dispatcher.h"
#include "protocol_message.h"
#include "room_manager.h"
#include "udp_relay.h"
#include "utils.h"

namespace {

constexpr const char* kVersion = "1.0";
constexpr int kSweepIntervalSec = 10;   // 每 10s 清扫一次
constexpr int kIdleTimeoutSec = 60;     // 超过 60s 无信令视为空闲

// RoomInfo -> JsonValue（ROOM_LIST / ROOM_INFO 用）
JsonValue roomToJson(const RoomInfo& info, bool withMembers) {
    JsonValue v = JsonValue::makeObject();
    v.set("roomId", JsonValue::makeString(info.roomId));
    v.set("roomName", JsonValue::makeString(info.roomName));
    v.set("ownerId", JsonValue::makeString(info.ownerId));
    v.set("memberCount", JsonValue::makeNumber(static_cast<double>(info.memberCount)));
    if (withMembers) {
        JsonValue members = JsonValue::makeArray();
        for (const auto& m : info.members) {
            JsonValue mv = JsonValue::makeObject();
            mv.set("userId", JsonValue::makeString(m.first));
            mv.set("userName", JsonValue::makeString(m.second));
            members.arr.push_back(std::move(mv));
        }
        v.set("members", std::move(members));
    }
    return v;
}

// ProtocolMessage -> JsonValue（需要附加额外字段时，先转 JsonValue 再 set/dump）
JsonValue msgToJson(const ProtocolMessage& m) {
    JsonValue root = JsonValue::makeObject();
    root.set("type", JsonValue::makeString(m.type));
    root.set("userId", JsonValue::makeString(m.userId));
    root.set("userName", JsonValue::makeString(m.userName));
    root.set("roomId", JsonValue::makeString(m.roomId));
    root.set("roomName", JsonValue::makeString(m.roomName));
    root.set("content", JsonValue::makeString(m.content));
    root.set("payload", JsonValue::makeString(m.payload));
    JsonValue extraVal = JsonValue::makeObject();
    for (const auto& kv : m.extra) extraVal.set(kv.first, JsonValue::makeString(kv.second));
    root.set("extra", std::move(extraVal));
    root.set("success", JsonValue::makeBool(m.success));
    root.set("timestamp", JsonValue::makeNumber(static_cast<double>(m.timestamp)));
    return root;
}

}  // namespace

int main(int argc, char** argv) {
    uint16_t port = 8888;
    if (argc >= 2) {
        int p = ::atoi(argv[1]);
        if (p > 0 && p < 65536) port = static_cast<uint16_t>(p);
    }

    ::signal(SIGPIPE, SIG_IGN);  // 防止写已关闭连接时被 SIGPIPE 杀死

    LOG_INFO("reactor server v%s starting, tcp port=%u, udp relay port=10001",
             kVersion, port);

    // ---- 组装组件（生命周期由 main 控制）----
    EventLoop loop;                          // epoll 事件循环（单线程 Reactor）
    RoomManager rooms;                       // 用户/房间状态（互斥锁保护）
    MessageDispatcher dispatcher;            // map + lambda 消息分发
    UdpRelay udp(&loop, 10001, &rooms);      // UDP/RTP 媒体中继
    std::atomic<bool> stopFlag{false};       // 心跳清扫线程退出标志

    // 房间广播的发送回调：fd -> ClientSession::sendLine（仅在 loop 线程调用）
    auto sendTo = [&loop](int fd, const std::string& data) {
        auto h = loop.getHandler(fd);
        if (h) static_cast<ClientSession*>(h.get())->sendLine(data);
    };

    // ---- 注册消息处理器（map + lambda，新增业务只加一行注册）----

    // 注册并登录
    dispatcher.registerHandler("REGISTER", [&](int fd, const ProtocolMessage& msg) {
        std::string err;
        bool ok = rooms.login(fd, msg.userId, msg.userName, err);
        auto reply = ProtocolMessage::makeReply("REGISTER_OK", msg, ok,
                                                ok ? "register ok" : err);
        sendTo(fd, reply.toJson());
    });

    // 登录
    dispatcher.registerHandler("LOGIN", [&](int fd, const ProtocolMessage& msg) {
        std::string err;
        bool ok = rooms.login(fd, msg.userId, msg.userName, err);
        auto reply = ProtocolMessage::makeReply("LOGIN_OK", msg, ok,
                                                ok ? "login ok" : err);
        sendTo(fd, reply.toJson());
    });

    // 创建房间（创建者自动加入）
    dispatcher.registerHandler("CREATE_ROOM", [&](int fd, const ProtocolMessage& msg) {
        std::string err;
        bool ok = rooms.createRoom(msg.roomId, msg.roomName, fd, err);
        auto reply = ProtocolMessage::makeReply("ROOM_CREATED", msg, ok,
                                                ok ? "room created" : err);
        sendTo(fd, reply.toJson());
    });

    // 加入房间，返回房间信息
    dispatcher.registerHandler("JOIN_ROOM", [&](int fd, const ProtocolMessage& msg) {
        std::string err;
        if (rooms.joinRoom(msg.roomId, fd, err)) {
            RoomInfo info;
            if (rooms.getRoomInfo(msg.roomId, info, err)) {
                auto reply = ProtocolMessage::makeReply("ROOM_INFO", msg, true, "join ok");
                JsonValue root = msgToJson(reply);
                root.set("roomInfo", roomToJson(info, true));
                sendTo(fd, root.dump());
                return;
            }
        }
        auto reply = ProtocolMessage::makeReply("ROOM_INFO", msg, false, err);
        sendTo(fd, reply.toJson());
    });

    // 离开房间
    dispatcher.registerHandler("LEAVE_ROOM", [&](int fd, const ProtocolMessage& msg) {
        std::string err;
        bool ok = rooms.leaveRoom(msg.roomId, fd, err);
        auto reply = ProtocolMessage::makeReply("LEAVE_ROOM_OK", msg, ok,
                                                ok ? "leave ok" : err);
        sendTo(fd, reply.toJson());
    });

    // 房间内聊天：广播给房间内其他人
    dispatcher.registerHandler("ROOM_CHAT", [&](int fd, const ProtocolMessage& msg) {
        std::string uid = rooms.userIdOf(fd);
        if (uid.empty()) {
            sendTo(fd, ProtocolMessage::makeReply("ERROR", msg, false, "not logged in").toJson());
            return;
        }
        ProtocolMessage out;
        out.type = "ROOM_MESSAGE";
        out.userId = uid;
        out.userName = rooms.userNameOf(fd);
        out.roomId = msg.roomId;
        out.content = msg.content;
        out.timestamp = static_cast<int64_t>(::time(nullptr));
        rooms.broadcastToRoom(msg.roomId, fd, out.toJson(), sendTo);
    });

    // 列出所有房间
    dispatcher.registerHandler("LIST_ROOMS", [&](int fd, const ProtocolMessage& msg) {
        auto list = rooms.listRooms();
        JsonValue arr = JsonValue::makeArray();
        for (const auto& info : list) arr.arr.push_back(roomToJson(info, false));
        auto reply = ProtocolMessage::makeReply("ROOM_LIST", msg, true, "");
        JsonValue root = msgToJson(reply);
        root.set("rooms", std::move(arr));
        sendTo(fd, root.dump());
    });

    // 查询指定房间信息
    dispatcher.registerHandler("ROOM_INFO", [&](int fd, const ProtocolMessage& msg) {
        std::string err;
        RoomInfo info;
        if (rooms.getRoomInfo(msg.roomId, info, err)) {
            auto reply = ProtocolMessage::makeReply("ROOM_INFO", msg, true, "ok");
            JsonValue root = msgToJson(reply);
            root.set("roomInfo", roomToJson(info, true));
            sendTo(fd, root.dump());
        } else {
            auto reply = ProtocolMessage::makeReply("ROOM_INFO", msg, false, err);
            sendTo(fd, reply.toJson());
        }
    });

    // 心跳保活
    dispatcher.registerHandler("PING", [&](int fd, const ProtocolMessage& msg) {
        auto reply = ProtocolMessage::makeReply("PONG", msg, true, "pong");
        sendTo(fd, reply.toJson());
    });

    // 登出并断开
    dispatcher.registerHandler("LOGOUT", [&](int fd, const ProtocolMessage&) {
        LOG_INFO("client fd=%d requested logout", fd);
        auto h = loop.getHandler(fd);
        if (h) static_cast<ClientSession*>(h.get())->closeSession();
    });

    // 注册本客户端 UDP 媒体地址，用于 RTP 转发
    dispatcher.registerHandler("MEDIA_REGISTER", [&](int fd, const ProtocolMessage& msg) {
        auto ipIt = msg.extra.find("ip");
        auto portIt = msg.extra.find("port");
        if (ipIt == msg.extra.end() || portIt == msg.extra.end()) {
            sendTo(fd, ProtocolMessage::makeReply("MEDIA_REGISTER_OK", msg, false,
                                                  "extra.ip / extra.port required").toJson());
            return;
        }
        int mediaPort = ::atoi(portIt->second.c_str());
        if (mediaPort <= 0 || mediaPort > 65535) {
            sendTo(fd, ProtocolMessage::makeReply("MEDIA_REGISTER_OK", msg, false,
                                                  "invalid media port").toJson());
            return;
        }
        udp.registerMedia(fd, msg.roomId, ipIt->second, static_cast<uint16_t>(mediaPort));
        auto reply = ProtocolMessage::makeReply("MEDIA_REGISTER_OK", msg, true,
                                                "media registered");
        reply.extra = msg.extra;  // 回显注册的媒体地址
        sendTo(fd, reply.toJson());
    });

    // SDP / ICE 透传：转发给房间内其他成员
    auto registerPeerForward = [&](const char* reqType, const char* peerType) {
        // 注意：handler 生命周期长于本函数栈帧，peerType 必须按值捕获（字符串字面量为静态存储期）
        dispatcher.registerHandler(reqType, [&, peerType](int fd, const ProtocolMessage& msg) {
            std::string uid = rooms.userIdOf(fd);
            if (uid.empty() || msg.payload.empty()) {
                sendTo(fd, ProtocolMessage::makeReply("ERROR", msg, false,
                                                      "not logged in or empty payload").toJson());
                return;
            }
            ProtocolMessage out;
            out.type = peerType;
            out.userId = uid;
            out.userName = rooms.userNameOf(fd);
            out.roomId = msg.roomId;
            out.payload = msg.payload;
            out.timestamp = static_cast<int64_t>(::time(nullptr));
            rooms.broadcastToRoom(msg.roomId, fd, out.toJson(), sendTo);
        });
    };
    registerPeerForward("SDP_OFFER", "PEER_SDP_OFFER");
    registerPeerForward("SDP_ANSWER", "PEER_SDP_ANSWER");
    registerPeerForward("ICE_CANDIDATE", "PEER_ICE_CANDIDATE");

    // 未注册消息类型
    dispatcher.setFallbackHandler([&](int fd, const ProtocolMessage& msg) {
        sendTo(fd, ProtocolMessage::makeReply("ERROR", msg, false,
                                              "unknown type: " + msg.type).toJson());
    });

    // TCP 监听（Acceptor 工厂创建 ClientSession）
    std::string err;
    Acceptor acceptor(&loop, port, &dispatcher, &rooms, &udp, err);
    if (!acceptor.ok()) {
        LOG_ERROR("%s", err.c_str());
        return 1;
    }
    if (!udp.ok()) {
        LOG_WARN("udp relay unavailable, media forwarding disabled");
    }

    // ---- 控制台线程：阻塞读取 stdin，stats/rooms/quit 命令经 runInLoop 进入 loop 线程 ----
    // 注意：不能用 poll + fgets 混用——stdio 会把管道数据一次读入用户态缓冲，
    // 之后 fd 层 poll 不可读（EOF 时仅报 POLLHUP），缓冲中剩余命令将永远无法读出
    std::thread console([&]() {
        char line[256];
        while (true) {
            if (!::fgets(line, sizeof(line), stdin)) break;  // EOF（如 </dev/null）或读错误

            std::string cmd(line);
            // trim
            while (!cmd.empty() && (cmd.back() == '\n' || cmd.back() == '\r' ||
                                    cmd.back() == ' ')) cmd.pop_back();

            if (cmd == "stats") {
                loop.runInLoop([&rooms] {
                    printf("online users: %zu, rooms: %zu\n",
                           rooms.onlineCount(), rooms.listRooms().size());
                    fflush(stdout);
                });
            } else if (cmd == "rooms") {
                loop.runInLoop([&rooms] {
                    JsonValue arr = JsonValue::makeArray();
                    for (const auto& info : rooms.listRooms()) {
                        JsonValue v = JsonValue::makeObject();
                        v.set("roomId", JsonValue::makeString(info.roomId));
                        v.set("roomName", JsonValue::makeString(info.roomName));
                        v.set("ownerId", JsonValue::makeString(info.ownerId));
                        v.set("memberCount",
                              JsonValue::makeNumber(static_cast<double>(info.memberCount)));
                        arr.arr.push_back(std::move(v));
                    }
                    printf("rooms: %s\n", arr.dump().c_str());
                    fflush(stdout);
                });
            } else if (cmd == "quit" || cmd == "exit") {
                loop.runInLoop([&loop] { loop.quit(); });
                break;
            } else if (!cmd.empty()) {
                printf("commands: stats | rooms | quit\n");
                fflush(stdout);
            }
        }
    });

    // ---- 心跳清扫线程：每 10s 投递一次清扫任务，关闭超过 60s 无信令的会话 ----
    std::thread sweeper([&]() {
        while (!stopFlag.load()) {
            std::this_thread::sleep_for(std::chrono::seconds(kSweepIntervalSec));
            if (stopFlag.load()) break;
            loop.runInLoop([&rooms, &loop] {
                for (int idleFd : rooms.sweepIdle(kIdleTimeoutSec)) {
                    auto h = loop.getHandler(idleFd);
                    if (h) static_cast<ClientSession*>(h.get())->closeSession();
                }
            });
        }
    });

    // ---- 运行事件循环 ----
    loop.loop();

    // 通知清扫线程退出；console 线程若阻塞在 stdin（交互终端）则随进程退出
    stopFlag.store(true);
    sweeper.join();
    console.detach();
    LOG_INFO("server exited");
    return 0;
}
