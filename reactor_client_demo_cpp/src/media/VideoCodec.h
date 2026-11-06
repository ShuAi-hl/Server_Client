#pragma once
// VideoCodec.h - 基于 FFmpeg 的 H.264 编码器 / 解码器
#include <QByteArray>
#include <QImage>
#include <QString>
#include <vector>

extern "C" {
struct AVCodecContext;
struct AVFrame;
struct AVPacket;
struct SwsContext;
}

// BGR24 原始帧 -> H.264 Annex-B 码流（每帧可能产出 0..n 个 NAL 单元包）
class H264Encoder {
public:
    ~H264Encoder();

    bool open(int width, int height, int fps, int bitrate);
    void close();
    bool isOpen() const { return ctx_ != nullptr; }

    // 输入 OpenCV BGR 图像（任意尺寸，内部缩放到目标尺寸）
    bool encode(const unsigned char* bgrData, int width, int height, int stride,
                std::vector<QByteArray>& packets, QString& err);

    int width() const { return width_; }
    int height() const { return height_; }

private:
    bool convertToYuv(const unsigned char* bgrData, int width, int height, int stride);
    bool drainPackets(std::vector<QByteArray>& packets);

    AVCodecContext* ctx_ = nullptr;
    AVFrame* frame_ = nullptr;
    AVPacket* pkt_ = nullptr;
    SwsContext* sws_ = nullptr;
    int width_ = 0;
    int height_ = 0;
    int fps_ = 30;
    qint64 pts_ = 0;
};

// H.264 Annex-B 码流 -> QImage（BGR888）
class H264Decoder {
public:
    ~H264Decoder();

    bool open();
    void close();
    bool isOpen() const { return ctx_ != nullptr; }

    // 送入一个完整访问单元；成功解出一帧时填充 image 并返回 true
    bool decode(const QByteArray& annexb, QImage& image, QString& err);

private:
    AVCodecContext* ctx_ = nullptr;
    AVFrame* frame_ = nullptr;
    AVPacket* pkt_ = nullptr;
    SwsContext* sws_ = nullptr;
    int swsW_ = 0;
    int swsH_ = 0;
};
