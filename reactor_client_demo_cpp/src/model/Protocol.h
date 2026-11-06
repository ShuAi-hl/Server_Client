#pragma once
// Protocol.h - 与 reactor_server 完全一致的信令消息结构
// 传输格式：每行一条 JSON，以 '\n' 结尾（TCP 文本协议）
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QMap>
#include <QString>

struct ProtocolMessage {
    QString type;                    // 消息类型：LOGIN / JOIN_ROOM / ROOM_CHAT ...
    QString userId;
    QString userName;
    QString roomId;
    QString roomName;
    QString content;                 // 聊天内容 / 说明
    QString payload;                 // SDP、ICE 候选等原始文本
    QMap<QString, QString> extra;    // 扩展字段（如媒体 ip/port）
    bool success = false;            // 服务端响应结果
    qint64 timestamp = 0;

    static ProtocolMessage fromJson(const QJsonObject& obj) {
        ProtocolMessage m;
        m.type = obj.value("type").toString();
        m.userId = obj.value("userId").toString();
        m.userName = obj.value("userName").toString();
        m.roomId = obj.value("roomId").toString();
        m.roomName = obj.value("roomName").toString();
        m.content = obj.value("content").toString();
        m.payload = obj.value("payload").toString();
        m.success = obj.value("success").toBool();
        m.timestamp = static_cast<qint64>(obj.value("timestamp").toDouble());

        const QJsonObject extraObj = obj.value("extra").toObject();
        for (auto it = extraObj.begin(); it != extraObj.end(); ++it) {
            if (it.value().isBool()) {
                m.extra.insert(it.key(), it.value().toBool() ? "true" : "false");
            } else if (it.value().isDouble()) {
                m.extra.insert(it.key(), QString::number(it.value().toDouble(), 'f', 0));
            } else {
                m.extra.insert(it.key(), it.value().toString());
            }
        }
        return m;
    }

    QJsonObject toJson() const {
        QJsonObject obj;
        obj.insert("type", type);
        obj.insert("userId", userId);
        obj.insert("userName", userName);
        obj.insert("roomId", roomId);
        obj.insert("roomName", roomName);
        obj.insert("content", content);
        obj.insert("payload", payload);

        QJsonObject extraObj;
        for (auto it = extra.begin(); it != extra.end(); ++it) {
            extraObj.insert(it.key(), it.value());
        }
        obj.insert("extra", extraObj);
        obj.insert("success", success);
        obj.insert("timestamp", static_cast<double>(timestamp));
        return obj;
    }
};
