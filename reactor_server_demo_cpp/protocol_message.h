#pragma once
// protocol_message.h - 协议消息 + 轻量 JSON（JsonValue）
#include <cstdint>
#include <map>
#include <string>
#include <utility>
#include <vector>

// 轻量 JSON 值：null / bool / number / string / array / object
// 内置解析与序列化，避免依赖外部库；如需完整功能可替换为 nlohmann/json
class JsonValue {
public:
    enum class Type { Null, Bool, Number, String, Array, Object };

    Type type = Type::Null;
    bool boolVal = false;
    double numVal = 0.0;
    std::string strVal;
    std::vector<JsonValue> arr;
    std::vector<std::pair<std::string, JsonValue>> obj;  // 保持插入顺序

    // 解析 JSON 文本；失败返回 Null 类型，err 输出原因
    static JsonValue parse(const std::string& text, std::string* err = nullptr);
    // 序列化为 JSON 文本
    std::string dump() const;

    // ---- 便捷方法 ----
    bool isNull() const { return type == Type::Null; }
    bool isString() const { return type == Type::String; }
    bool isObject() const { return type == Type::Object; }
    bool isArray() const { return type == Type::Array; }
    bool isNumber() const { return type == Type::Number; }

    static JsonValue makeString(const std::string& s) {
        JsonValue v; v.type = Type::String; v.strVal = s; return v;
    }
    static JsonValue makeNumber(double d) {
        JsonValue v; v.type = Type::Number; v.numVal = d; return v;
    }
    static JsonValue makeBool(bool b) {
        JsonValue v; v.type = Type::Bool; v.boolVal = b; return v;
    }
    static JsonValue makeArray() {
        JsonValue v; v.type = Type::Array; return v;
    }
    static JsonValue makeObject() {
        JsonValue v; v.type = Type::Object; return v;
    }

    void set(const std::string& key, JsonValue value) {
        for (auto& kv : obj) {
            if (kv.first == key) { kv.second = std::move(value); return; }
        }
        obj.emplace_back(key, std::move(value));
    }

    // 查找 object 成员，不存在返回 nullptr
    const JsonValue* find(const std::string& key) const {
        if (type != Type::Object) return nullptr;
        for (auto& kv : obj) {
            if (kv.first == key) return &kv.second;
        }
        return nullptr;
    }

    // 取 string 成员（不存在或类型不符返回默认值）
    std::string getString(const std::string& key, const std::string& def = "") const {
        const JsonValue* v = find(key);
        return (v && v->isString()) ? v->strVal : def;
    }

    // 取 number 成员
    double getNumber(const std::string& key, double def = 0.0) const {
        const JsonValue* v = find(key);
        return (v && v->isNumber()) ? v->numVal : def;
    }
};

// 业务协议消息（每行一条 JSON，以 \n 结尾）
struct ProtocolMessage {
    std::string type;                                 // 消息类型，如 LOGIN
    std::string userId;                               // 用户 ID
    std::string userName;                             // 用户名
    std::string roomId;                               // 房间 ID
    std::string roomName;                             // 房间名
    std::string content;                              // 聊天内容/说明
    std::string payload;                              // SDP/ICE 等原始数据
    std::map<std::string, std::string> extra;         // 扩展字段（如媒体 ip/port）
    bool success = false;                             // S->C 请求结果
    int64_t timestamp = 0;                            // 时间戳（秒）

    // 序列化为一行 JSON（不含末尾 \n）
    std::string toJson() const;
    // 从一行 JSON 反序列化；失败返回 false 且 err 给出原因
    static bool fromJson(const std::string& line, ProtocolMessage& out, std::string& err);

    // 构造一条响应消息（回填请求的 userId/roomId 等上下文）
    static ProtocolMessage makeReply(const char* type, const ProtocolMessage& req,
                                     bool success, const std::string& content);
};
