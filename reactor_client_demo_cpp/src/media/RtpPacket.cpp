// RtpPacket.cpp - RTP 封装与重组实现
#include "RtpPacket.h"

#include <QRandomGenerator>
#include <QtEndian>

#include <cstring>

namespace rtp {
namespace {

inline void writeU8(char* p, quint8 v) { p[0] = static_cast<char>(v); }
inline void writeU16(char* p, quint16 v) { qToBigEndian<quint16>(v, reinterpret_cast<uchar*>(p)); }
inline void writeU32(char* p, quint32 v) { qToBigEndian<quint32>(v, reinterpret_cast<uchar*>(p)); }

inline quint8 readU8(const char* p) { return static_cast<quint8>(p[0]); }
inline quint16 readU16(const char* p) { return qFromBigEndian<quint16>(reinterpret_cast<const uchar*>(p)); }
inline quint32 readU32(const char* p) { return qFromBigEndian<quint32>(reinterpret_cast<const uchar*>(p)); }

}  // namespace

QByteArray buildPacket(const Header& h, const char* payload, int payloadLen) {
    QByteArray out(kHeaderSize + payloadLen, Qt::Uninitialized);
    char* p = out.data();

    // RTP 头：V=2, P=0, X=0, CC=0
    writeU8(p + 0, 0x80);
    writeU8(p + 1, static_cast<quint8>((h.marker ? 0x80 : 0x00) | (h.payloadType & 0x7F)));
    writeU16(p + 2, h.sequence);
    writeU32(p + 4, h.timestamp);
    writeU32(p + 8, h.ssrc);

    // 自定义分片头
    writeU32(p + 12, h.frameId);
    writeU32(p + 16, h.offset);
    writeU32(p + 20, h.frameSize);

    if (payloadLen > 0) {
        std::memcpy(p + kHeaderSize, payload, static_cast<size_t>(payloadLen));
    }
    return out;
}

bool parsePacket(const QByteArray& data, Header& header) {
    if (data.size() < kHeaderSize) return false;

    const char* p = data.constData();
    const quint8 b0 = readU8(p);
    if ((b0 >> 6) != 2) return false;       // 版本必须为 2
    const quint8 cc = b0 & 0x0F;
    const bool padding = (b0 & 0x20) != 0;
    const bool extension = (b0 & 0x10) != 0;
    if (cc != 0 || padding || extension) return false;  // 本协议不使用这些字段

    header.marker = (readU8(p + 1) & 0x80) != 0;
    header.payloadType = readU8(p + 1) & 0x7F;
    header.sequence = readU16(p + 2);
    header.timestamp = readU32(p + 4);
    header.ssrc = readU32(p + 8);
    header.frameId = readU32(p + 12);
    header.offset = readU32(p + 16);
    header.frameSize = readU32(p + 20);
    header.payloadSize = data.size() - kHeaderSize;

    if (header.frameSize == 0) return false;
    if (header.offset + static_cast<quint32>(header.payloadSize) > header.frameSize) return false;
    return true;
}

bool Reassembler::push(const Header& header, const QByteArray& payload, QByteArray& complete) {
    if (!hasFrame_ || current_.frameId != header.frameId) {
        // 新的一帧：丢弃上一帧残留（丢包或乱序，实时场景直接丢弃更划算）
        current_ = FrameState();
        current_.frameId = header.frameId;
        current_.frameSize = header.frameSize;
        hasFrame_ = true;
    }

    if (current_.chunks.contains(header.offset)) return false;  // 重复分片
    current_.chunks.insert(header.offset, payload);
    current_.received += payload.size();

    if (current_.received >= static_cast<int>(current_.frameSize)) {
        complete.clear();
        complete.reserve(static_cast<int>(current_.frameSize));
        for (auto it = current_.chunks.begin(); it != current_.chunks.end(); ++it) {
            complete.append(it.value());
        }
        hasFrame_ = false;
        current_ = FrameState();
        return true;
    }
    return false;
}

void Reassembler::reset() {
    current_ = FrameState();
    hasFrame_ = false;
}

quint32 makeSsrc() {
    return QRandomGenerator::global()->generate();
}

}  // namespace rtp
