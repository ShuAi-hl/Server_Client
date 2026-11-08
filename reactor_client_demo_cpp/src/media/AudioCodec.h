#pragma once
// AudioCodec.h - 基于 FFmpeg 的 AAC 编码器 / 解码器
// 统一采样格式：48000Hz / 双声道 / S16 交错（与 QAudioFormat 对应）
#include <QByteArray>
#include <QString>
#include <vector>

extern "C" {
struct AVCodecContext;
struct AVFrame;
struct AVPacket;
struct SwrContext;
struct AVChannelLayout;
}

class AacEncoder {
public:
    ~AacEncoder();

    bool open(int sampleRate, int channels, int bitrate);
    void close();
    bool isOpen() const { return ctx_ != nullptr; }

    int sampleRate() const { return sampleRate_; }
    int channels() const { return channels_; }
    int frameSamples() const { return frameSamples_; }

    // 输入任意长度的 S16 交错 PCM，输出完整 AAC 帧（每帧 1024 采样）
    void feed(const QByteArray& pcmS16, std::vector<QByteArray>& packets);
    // 冲洗编码器内部剩余数据
    void flush(std::vector<QByteArray>& packets);

private:
    bool encodeOneFrame(std::vector<QByteArray>& packets);
    bool drain(std::vector<QByteArray>& packets);

    AVCodecContext* ctx_ = nullptr;
    AVFrame* frame_ = nullptr;
    AVPacket* pkt_ = nullptr;
    SwrContext* swr_ = nullptr;
    int sampleRate_ = 48000;
    int channels_ = 2;
    int frameSamples_ = 1024;
    qint64 pts_ = 0;
    QByteArray pending_;  // 尚未凑满一帧的 PCM
};

class AacDecoder {
public:
    ~AacDecoder();

    bool open();
    void close();
    bool isOpen() const { return ctx_ != nullptr; }

    // 输入一个 AAC 帧，输出 S16 交错 PCM（48000Hz 双声道）
    bool decode(const QByteArray& aac, QByteArray& pcmS16, QString& err);

private:
    AVCodecContext* ctx_ = nullptr;
    AVFrame* frame_ = nullptr;
    AVPacket* pkt_ = nullptr;
    SwrContext* swr_ = nullptr;
    int swrInFmt_ = -1;
    int swrInRate_ = 0;
    int swrInCh_ = 0;
};

// 采集端采样格式（与 QAudioFormat::SampleFormat 对应）
enum class PcmSampleFormat { U8, S16, S32, F32 };

// 任意采样率/声道数/采样格式的 PCM -> 目标采样率/声道数/采样格式（默认 48kHz 双声道 S16）
// 采集方向用于把设备格式对齐到编码器；播放方向用于把解码结果对齐到扬声器格式
class PcmResampler {
public:
    ~PcmResampler();

    bool open(int inSampleRate, int inChannels, PcmSampleFormat inFormat,
              int outSampleRate = 48000, int outChannels = 2,
              PcmSampleFormat outFormat = PcmSampleFormat::S16);
    void close();
    bool isOpen() const { return swr_ != nullptr; }

    // 输入交错 PCM，输出为目标格式；失败时返回空
    QByteArray convert(const QByteArray& input);

private:
    SwrContext* swr_ = nullptr;
    int inRate_ = 48000;
    int inChannels_ = 2;
    PcmSampleFormat inFormat_ = PcmSampleFormat::S16;
    int outChannels_ = 2;
    PcmSampleFormat outFormat_ = PcmSampleFormat::S16;
};
