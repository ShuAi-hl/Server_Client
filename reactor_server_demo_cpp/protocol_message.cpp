// protocol_message.cpp - 轻量 JSON 解析/序列化 + 协议消息实现
#include "protocol_message.h"

#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <ctime>

// ============================ JsonValue ============================

namespace {

// 递归下降解析器
class JsonParser {
public:
    JsonParser(const char* begin, const char* end) : cur_(begin), end_(end) {}

    bool parse(JsonValue& out, std::string& err) {
        skipSpace();
        if (!parseValue(out, err, 0)) return false;
        skipSpace();
        if (cur_ != end_) {
            err = "trailing characters after JSON value";
            return false;
        }
        return true;
    }

private:
    const char* cur_;
    const char* end_;

    void skipSpace() {
        while (cur_ < end_ && ::isspace(static_cast<unsigned char>(*cur_))) ++cur_;
    }

    bool peek(char& c) const {
        if (cur_ >= end_) return false;
        c = *cur_;
        return true;
    }

    bool parseValue(JsonValue& out, std::string& err, int depth) {
        if (depth > 64) {
            err = "nesting too deep";
            return false;
        }
        char c;
        if (!peek(c)) {
            err = "unexpected end of input";
            return false;
        }
        switch (c) {
            case '{': return parseObject(out, err, depth);
            case '[': return parseArray(out, err, depth);
            case '"': out.type = JsonValue::Type::String;
                      return parseString(out.strVal, err);
            case 't': return parseLiteral(out, err, "true", JsonValue::Type::Bool, true);
            case 'f': return parseLiteral(out, err, "false", JsonValue::Type::Bool, false);
            case 'n': return parseLiteral(out, err, "null", JsonValue::Type::Null, false);
            default:  return parseNumber(out, err);
        }
    }

    bool parseLiteral(JsonValue& out, std::string& err, const char* lit,
                      JsonValue::Type type, bool boolVal) {
        size_t len = ::strlen(lit);
        if (end_ - cur_ < static_cast<long>(len) || ::strncmp(cur_, lit, len) != 0) {
            err = std::string("invalid literal, expected ") + lit;
            return false;
        }
        cur_ += len;
        out.type = type;
        out.boolVal = boolVal;
        return true;
    }

    bool parseNumber(JsonValue& out, std::string& err) {
        const char* start = cur_;
        if (cur_ < end_ && *cur_ == '-') ++cur_;
        while (cur_ < end_ &&
               (::isdigit(static_cast<unsigned char>(*cur_)) || *cur_ == '.' ||
                *cur_ == 'e' || *cur_ == 'E' || *cur_ == '+' || *cur_ == '-')) {
            ++cur_;
        }
        if (cur_ == start) {
            err = "invalid value";
            return false;
        }
        char buf[64];
        size_t len = static_cast<size_t>(cur_ - start);
        if (len >= sizeof(buf)) {
            err = "number too long";
            return false;
        }
        ::memcpy(buf, start, len);
        buf[len] = '\0';
        char* tail = nullptr;
        double d = ::strtod(buf, &tail);
        if (tail != buf + len) {
            err = "invalid number";
            return false;
        }
        out.type = JsonValue::Type::Number;
        out.numVal = d;
        return true;
    }

    // 解析字符串字面量（含转义），结果写入 out
    bool parseString(std::string& out, std::string& err) {
        if (!consume('"')) {
            err = "expected '\"'";
            return false;
        }
        out.clear();
        while (cur_ < end_) {
            unsigned char c = static_cast<unsigned char>(*cur_++);
            if (c == '"') return true;
            if (c == '\\') {
                if (cur_ >= end_) break;
                char esc = *cur_++;
                switch (esc) {
                    case '"':  out.push_back('"');  break;
                    case '\\': out.push_back('\\'); break;
                    case '/':  out.push_back('/');  break;
                    case 'b':  out.push_back('\b'); break;
                    case 'f':  out.push_back('\f'); break;
                    case 'n':  out.push_back('\n'); break;
                    case 'r':  out.push_back('\r'); break;
                    case 't':  out.push_back('\t'); break;
                    case 'u': {
                        uint32_t cp = 0;
                        if (!parseHex4(cp, err)) return false;
                        // 代理对：高代理后跟 \uDC00-\uDFFF 低代理
                        if (cp >= 0xD800 && cp <= 0xDBFF) {
                            if (end_ - cur_ >= 6 && cur_[0] == '\\' && cur_[1] == 'u') {
                                cur_ += 2;
                                uint32_t lo = 0;
                                if (!parseHex4(lo, err)) return false;
                                if (lo >= 0xDC00 && lo <= 0xDFFF) {
                                    cp = 0x10000 + ((cp - 0xD800) << 10) + (lo - 0xDC00);
                                } else {
                                    err = "invalid surrogate pair";
                                    return false;
                                }
                            } else {
                                err = "unpaired surrogate";
                                return false;
                            }
                        } else if (cp >= 0xDC00 && cp <= 0xDFFF) {
                            err = "unpaired surrogate";
                            return false;
                        }
                        appendUtf8(out, cp);
                        break;
                    }
                    default:
                        err = std::string("invalid escape '\\") + esc + "'";
                        return false;
                }
            } else {
                out.push_back(static_cast<char>(c));  // UTF-8 多字节原样保留（支持中文）
            }
        }
        err = "unterminated string";
        return false;
    }

    bool parseHex4(uint32_t& out, std::string& err) {
        if (end_ - cur_ < 4) {
            err = "invalid \\u escape";
            return false;
        }
        out = 0;
        for (int i = 0; i < 4; ++i) {
            char c = *cur_++;
            out <<= 4;
            if (c >= '0' && c <= '9')      out |= static_cast<uint32_t>(c - '0');
            else if (c >= 'a' && c <= 'f') out |= static_cast<uint32_t>(c - 'a' + 10);
            else if (c >= 'A' && c <= 'F') out |= static_cast<uint32_t>(c - 'A' + 10);
            else {
                err = "invalid hex digit in \\u escape";
                return false;
            }
        }
        return true;
    }

    static void appendUtf8(std::string& out, uint32_t cp) {
        if (cp < 0x80) {
            out.push_back(static_cast<char>(cp));
        } else if (cp < 0x800) {
            out.push_back(static_cast<char>(0xC0 | (cp >> 6)));
            out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        } else if (cp < 0x10000) {
            out.push_back(static_cast<char>(0xE0 | (cp >> 12)));
            out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        } else {
            out.push_back(static_cast<char>(0xF0 | (cp >> 18)));
            out.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        }
    }

    bool parseArray(JsonValue& out, std::string& err, int depth) {
        if (!consume('[')) {
            err = "expected '['";
            return false;
        }
        out.type = JsonValue::Type::Array;
        skipSpace();
        char c;
        if (peek(c) && c == ']') {
            ++cur_;
            return true;
        }
        while (true) {
            JsonValue item;
            skipSpace();
            if (!parseValue(item, err, depth + 1)) return false;
            out.arr.push_back(std::move(item));
            skipSpace();
            if (!peek(c)) {
                err = "unterminated array";
                return false;
            }
            if (c == ',') { ++cur_; continue; }
            if (c == ']') { ++cur_; return true; }
            err = "expected ',' or ']' in array";
            return false;
        }
    }

    bool parseObject(JsonValue& out, std::string& err, int depth) {
        if (!consume('{')) {
            err = "expected '{'";
            return false;
        }
        out.type = JsonValue::Type::Object;
        skipSpace();
        char c;
        if (peek(c) && c == '}') {
            ++cur_;
            return true;
        }
        while (true) {
            std::string key;
            skipSpace();
            if (!parseString(key, err)) return false;
            skipSpace();
            if (!consume(':')) {
                err = "expected ':' in object";
                return false;
            }
            JsonValue value;
            skipSpace();
            if (!parseValue(value, err, depth + 1)) return false;
            out.obj.emplace_back(std::move(key), std::move(value));
            skipSpace();
            if (!peek(c)) {
                err = "unterminated object";
                return false;
            }
            if (c == ',') { ++cur_; continue; }
            if (c == '}') { ++cur_; return true; }
            err = "expected ',' or '}' in object";
            return false;
        }
    }

    bool consume(char c) {
        if (cur_ < end_ && *cur_ == c) {
            ++cur_;
            return true;
        }
        return false;
    }
};

void dumpString(const std::string& s, std::string& out) {
    out.push_back('"');
    for (unsigned char c : s) {
        switch (c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\b': out += "\\b";  break;
            case '\f': out += "\\f";  break;
            case '\n': out += "\\n";  break;
            case '\r': out += "\\r";  break;
            case '\t': out += "\\t";  break;
            default:
                if (c < 0x20) {
                    char buf[8];
                    ::snprintf(buf, sizeof(buf), "\\u%04x", c);
                    out += buf;
                } else {
                    out.push_back(static_cast<char>(c));  // UTF-8 中文原样输出
                }
        }
    }
    out.push_back('"');
}

void dumpNumber(double d, std::string& out) {
    if (::std::isfinite(d) && d == static_cast<long long>(d) &&
        ::std::fabs(d) < 1e15) {
        char buf[32];
        ::snprintf(buf, sizeof(buf), "%lld", static_cast<long long>(d));
        out += buf;
    } else {
        char buf[32];
        ::snprintf(buf, sizeof(buf), "%g", d);
        out += buf;
    }
}

}  // namespace

JsonValue JsonValue::parse(const std::string& text, std::string* err) {
    JsonValue out;
    std::string localErr;
    JsonParser parser(text.data(), text.data() + text.size());
    if (!parser.parse(out, localErr)) {
        if (err) *err = localErr;
        return JsonValue();  // Null
    }
    if (err) err->clear();
    return out;
}

std::string JsonValue::dump() const {
    std::string out;
    switch (type) {
        case Type::Null:   out += "null"; break;
        case Type::Bool:   out += boolVal ? "true" : "false"; break;
        case Type::Number: dumpNumber(numVal, out); break;
        case Type::String: dumpString(strVal, out); break;
        case Type::Array:
            out.push_back('[');
            for (size_t i = 0; i < arr.size(); ++i) {
                if (i > 0) out.push_back(',');
                out += arr[i].dump();
            }
            out.push_back(']');
            break;
        case Type::Object:
            out.push_back('{');
            for (size_t i = 0; i < obj.size(); ++i) {
                if (i > 0) out.push_back(',');
                dumpString(obj[i].first, out);
                out.push_back(':');
                out += obj[i].second.dump();
            }
            out.push_back('}');
            break;
    }
    return out;
}

// ========================= ProtocolMessage =========================

std::string ProtocolMessage::toJson() const {
    JsonValue root = JsonValue::makeObject();
    root.set("type", JsonValue::makeString(type));
    root.set("userId", JsonValue::makeString(userId));
    root.set("userName", JsonValue::makeString(userName));
    root.set("roomId", JsonValue::makeString(roomId));
    root.set("roomName", JsonValue::makeString(roomName));
    root.set("content", JsonValue::makeString(content));
    root.set("payload", JsonValue::makeString(payload));

    JsonValue extraVal = JsonValue::makeObject();
    for (const auto& kv : extra) {
        extraVal.set(kv.first, JsonValue::makeString(kv.second));
    }
    root.set("extra", std::move(extraVal));

    root.set("success", JsonValue::makeBool(success));
    root.set("timestamp", JsonValue::makeNumber(static_cast<double>(timestamp)));
    return root.dump();
}

bool ProtocolMessage::fromJson(const std::string& line, ProtocolMessage& out, std::string& err) {
    JsonValue root = JsonValue::parse(line, &err);
    if (root.isNull()) {
        if (err.empty()) err = "invalid JSON";
        return false;
    }

    out.type = root.getString("type");
    if (out.type.empty()) {
        err = "missing field \"type\"";
        return false;
    }
    out.userId = root.getString("userId");
    out.userName = root.getString("userName");
    out.roomId = root.getString("roomId");
    out.roomName = root.getString("roomName");
    out.content = root.getString("content");
    out.payload = root.getString("payload");
    out.success = root.getNumber("success") != 0;  // 兼容 true/false 与 1/0
    if (const JsonValue* v = root.find("success")) {
        if (v->type == JsonValue::Type::Bool) out.success = v->boolVal;
    }
    out.timestamp = static_cast<int64_t>(root.getNumber("timestamp"));

    if (const JsonValue* extraVal = root.find("extra")) {
        if (extraVal->isObject()) {
            for (const auto& kv : extraVal->obj) {
                switch (kv.second.type) {
                    case JsonValue::Type::String:  out.extra[kv.first] = kv.second.strVal; break;
                    case JsonValue::Type::Number:  out.extra[kv.first] = kv.second.dump(); break;
                    case JsonValue::Type::Bool:    out.extra[kv.first] = kv.second.boolVal ? "true" : "false"; break;
                    default:                       out.extra[kv.first] = kv.second.dump(); break;
                }
            }
        }
    }
    return true;
}

ProtocolMessage ProtocolMessage::makeReply(const char* type, const ProtocolMessage& req,
                                           bool success, const std::string& content) {
    ProtocolMessage reply;
    reply.type = type;
    reply.userId = req.userId;
    reply.userName = req.userName;
    reply.roomId = req.roomId;
    reply.roomName = req.roomName;
    reply.content = content;
    reply.success = success;
    reply.timestamp = static_cast<int64_t>(::time(nullptr));
    return reply;
}
