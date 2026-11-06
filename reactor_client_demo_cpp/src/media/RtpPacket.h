#pragma once
// RtpPacket.h - RTP 封装与重组
//
// 报文布局（与 reactor_server 的「原样转发」配合）：
//   ┌──────────── RTP 头 12 字节 ────────────┬──── 自定义分片头 12 字节 ────┬── 载荷 ──┐
//   │ V/P/X/CC(1) M/PT(1) seq(2) ts(4) ssrc(4) │ frameId(4) offset(4) size(4) │ bytes   │
//   └────────────────────────────────────────┴─────────────────────────────┴─────────┘
//
// 有标准 RTP 头（V=2，PT 96=视频 / 97=音频，marker 表示一帧结束），
// 分片头用于把一帧 H.264 / AAC 数据拆成多个小于 MTU 的数据报并在接收端重组。
#include <QByteArray>
#include <QMap>
#include <QtGlobal>

namespace rtp {

constexpr quint8 kPayloadTypeVideo = 96;
constexpr quint8 kPayloadTypeAudio = 97;

constexpr int kRtpHeaderSize = 12;
constexpr int kFragHeaderSize = 12;
constexpr int kHeaderSize = kRtpHeaderSize + kFragHeaderSize;

// 单包载荷上限：24 字节头 + 1200 = 1224，留出 IP/UDP 开销后仍在 1500 MTU 内
constexpr int kMaxPayloadSize = 1200;

struct Header {
    quint8 payloadType = 0;
    bool marker = false;
    quint16 sequence = 0;
    quint32 timestamp = 0;
    quint32 ssrc = 0;
    quint32 frameId = 0;
    quint32 offset = 0;
    quint32 frameSize = 0;
    int payloadSize = 0;  // 由 parse 回填
};

// 把一个分片组装成一个完整数据报
QByteArray buildPacket(const Header& h, const char* payload, int payloadLen);

// 解析收到的数据报；失败返回 false（非本协议或损坏包）
bool parsePacket(const QByteArray& data, Header& header);

// 按帧重组：同一 SSRC 的分片累积成完整帧
class Reassembler {
public:
    // 推入一个分片；当该帧收齐时通过 complete 返回完整数据并重置状态
    bool push(const Header& header, const QByteArray& payload, QByteArray& complete);

    void reset();

private:
    struct FrameState {
        quint32 frameId = 0;
        quint32 frameSize = 0;
        int received = 0;
        QMap<quint32, QByteArray> chunks;  // offset -> 数据
    };

    FrameState current_;
    bool hasFrame_ = false;
};

// 生成 ssrc（按进程随机，保证多客户端不同）
quint32 makeSsrc();

}  // namespace rtp
