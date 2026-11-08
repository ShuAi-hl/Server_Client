// AudioCodec.cpp - FFmpeg AAC 编解码实现（FFmpeg 7.x API）
// 约定：编码输入 / 解码输出统一为 48000Hz、双声道、S16 交错 PCM
#include "AudioCodec.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/mathematics.h>
#include <libavutil/opt.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>
}

#include <cstring>

// ============================ 编码器 ============================

AacEncoder::~AacEncoder() { close(); }

bool AacEncoder::open(int sampleRate, int channels, int bitrate) {
    close();

    const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_AAC);
    if (!codec) return false;

    sampleRate_ = sampleRate > 0 ? sampleRate : 48000;
    channels_ = channels == 1 ? 1 : 2;

    ctx_ = avcodec_alloc_context3(codec);
    if (!ctx_) return false;

    ctx_->sample_rate = sampleRate_;
    av_channel_layout_default(&ctx_->ch_layout, channels_);
    ctx_->sample_fmt = AV_SAMPLE_FMT_FLTP;   // AAC 编码器只接受平面浮点
    ctx_->bit_rate = bitrate > 0 ? bitrate : 128000;
    ctx_->time_base = {1, sampleRate_};
    ctx_->profile = AV_PROFILE_AAC_LOW;

    if (avcodec_open2(ctx_, codec, nullptr) < 0) {
        close();
        return false;
    }

    frameSamples_ = ctx_->frame_size > 0 ? ctx_->frame_size : 1024;

    frame_ = av_frame_alloc();
    pkt_ = av_packet_alloc();
    if (!frame_ || !pkt_) {
        close();
        return false;
    }

    frame_->format = AV_SAMPLE_FMT_FLTP;
    frame_->sample_rate = sampleRate_;
    av_channel_layout_copy(&frame_->ch_layout, &ctx_->ch_layout);
    frame_->nb_samples = frameSamples_;
    if (av_frame_get_buffer(frame_, 0) < 0) {
        close();
        return false;
    }

    // S16 交错 -> FLTP 平面（同采样率、同声道数，只做格式转换）
    AVChannelLayout inLayout;
    av_channel_layout_default(&inLayout, channels_);
    const int ret = swr_alloc_set_opts2(&swr_, &ctx_->ch_layout, AV_SAMPLE_FMT_FLTP,
                                        sampleRate_, &inLayout, AV_SAMPLE_FMT_S16,
                                        sampleRate_, 0, nullptr);
    av_channel_layout_uninit(&inLayout);
    if (ret < 0 || !swr_ || swr_init(swr_) < 0) {
        close();
        return false;
    }
    return true;
}

void AacEncoder::close() {
    if (swr_) { swr_free(&swr_); }
    if (frame_) { av_frame_free(&frame_); }
    if (pkt_) { av_packet_free(&pkt_); }
    if (ctx_) { avcodec_free_context(&ctx_); }
    pending_.clear();
    pts_ = 0;
}

bool AacEncoder::drain(std::vector<QByteArray>& packets) {
    while (true) {
        const int ret = avcodec_receive_packet(ctx_, pkt_);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) return true;
        if (ret < 0) return false;
        packets.emplace_back(QByteArray(reinterpret_cast<const char*>(pkt_->data), pkt_->size));
        av_packet_unref(pkt_);
    }
}

bool AacEncoder::encodeOneFrame(std::vector<QByteArray>& packets) {
    frame_->pts = pts_;
    pts_ += frame_->nb_samples;

    if (avcodec_send_frame(ctx_, frame_) < 0) return false;
    return drain(packets);
}

void AacEncoder::feed(const QByteArray& pcmS16, std::vector<QByteArray>& packets) {
    if (!isOpen() || pcmS16.isEmpty()) return;

    pending_.append(pcmS16);

    // 每帧消耗 frameSamples_ * channels * 2 字节 S16 数据
    const int bytesPerFrame = frameSamples_ * channels_ * 2;
    while (pending_.size() >= bytesPerFrame) {
        if (av_frame_make_writable(frame_) < 0) break;

        const uint8_t* in[1] = { reinterpret_cast<const uint8_t*>(pending_.constData()) };
        const int got = swr_convert(swr_, frame_->data, frameSamples_, in, frameSamples_);
        if (got <= 0) break;

        pending_.remove(0, bytesPerFrame);
        frame_->nb_samples = got;
        encodeOneFrame(packets);
    }
}

void AacEncoder::flush(std::vector<QByteArray>& packets) {
    if (!isOpen()) return;
    avcodec_send_frame(ctx_, nullptr);
    drain(packets);
}

// ============================ 解码器 ============================

AacDecoder::~AacDecoder() { close(); }

bool AacDecoder::open() {
    close();

    const AVCodec* codec = avcodec_find_decoder(AV_CODEC_ID_AAC);
    if (!codec) return false;

    ctx_ = avcodec_alloc_context3(codec);
    if (!ctx_) return false;

    if (avcodec_open2(ctx_, codec, nullptr) < 0) {
        close();
        return false;
    }

    frame_ = av_frame_alloc();
    pkt_ = av_packet_alloc();
    if (!frame_ || !pkt_) {
        close();
        return false;
    }
    return true;
}

void AacDecoder::close() {
    if (swr_) { swr_free(&swr_); }
    swrInFmt_ = -1;
    swrInRate_ = 0;
    swrInCh_ = 0;
    if (frame_) { av_frame_free(&frame_); }
    if (pkt_) { av_packet_free(&pkt_); }
    if (ctx_) { avcodec_free_context(&ctx_); }
}

bool AacDecoder::decode(const QByteArray& aac, QByteArray& pcmS16, QString& err) {
    pcmS16.clear();
    if (!isOpen() || aac.isEmpty()) return false;

    av_packet_unref(pkt_);
    if (av_new_packet(pkt_, aac.size()) < 0) {
        err = QStringLiteral("分配解码包失败");
        return false;
    }
    std::memcpy(pkt_->data, aac.constData(), static_cast<size_t>(aac.size()));

    if (avcodec_send_packet(ctx_, pkt_) < 0) {
        err = QStringLiteral("AAC 解码 send_packet 失败");
        return false;
    }

    const int ret = avcodec_receive_frame(ctx_, frame_);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) return false;
    if (ret < 0) {
        err = QStringLiteral("AAC 解码 receive_frame 失败：%1").arg(ret);
        return false;
    }

    // 输入格式（采样率 / 声道数 / 采样格式）可能随流变化，需要重建重采样上下文
    if (!swr_ || swrInFmt_ != frame_->format ||
        swrInRate_ != frame_->sample_rate || swrInCh_ != frame_->ch_layout.nb_channels) {
        if (swr_) swr_free(&swr_);

        AVChannelLayout inLayout;
        av_channel_layout_copy(&inLayout, &frame_->ch_layout);
        AVChannelLayout outLayout;
        av_channel_layout_default(&outLayout, 2);

        const int r = swr_alloc_set_opts2(&swr_, &outLayout, AV_SAMPLE_FMT_S16, 48000,
                                          &inLayout, static_cast<AVSampleFormat>(frame_->format),
                                          frame_->sample_rate, 0, nullptr);
        av_channel_layout_uninit(&inLayout);
        av_channel_layout_uninit(&outLayout);
        if (r < 0 || !swr_ || swr_init(swr_) < 0) {
            if (swr_) swr_free(&swr_);
            err = QStringLiteral("音频重采样上下文创建失败");
            return false;
        }
        swrInFmt_ = frame_->format;
        swrInRate_ = frame_->sample_rate;
        swrInCh_ = frame_->ch_layout.nb_channels;
    }

    const int outSamples = static_cast<int>(
        av_rescale_rnd(swr_get_delay(swr_, frame_->sample_rate) + frame_->nb_samples,
                       48000, frame_->sample_rate, AV_ROUND_UP));
    if (outSamples <= 0) return false;

    pcmS16.resize(outSamples * 2 * 2);  // 双声道 * S16
    uint8_t* out[1] = { reinterpret_cast<uint8_t*>(pcmS16.data()) };
    const int got = swr_convert(swr_, out, outSamples,
                                const_cast<const uint8_t**>(frame_->extended_data),
                                frame_->nb_samples);
    if (got <= 0) {
        pcmS16.clear();
        return false;
    }
    pcmS16.resize(got * 2 * 2);
    return true;
}

// ============================ 重采样 ============================

namespace {

AVSampleFormat toAvFormat(PcmSampleFormat fmt) {
    switch (fmt) {
        case PcmSampleFormat::U8: return AV_SAMPLE_FMT_U8;
        case PcmSampleFormat::S32: return AV_SAMPLE_FMT_S32;
        case PcmSampleFormat::F32: return AV_SAMPLE_FMT_FLT;
        case PcmSampleFormat::S16: break;
    }
    return AV_SAMPLE_FMT_S16;
}

int bytesPerSampleOf(PcmSampleFormat fmt) {
    switch (fmt) {
        case PcmSampleFormat::U8: return 1;
        case PcmSampleFormat::S32: return 4;
        case PcmSampleFormat::F32: return 4;
        case PcmSampleFormat::S16: break;
    }
    return 2;
}

}  // namespace

PcmResampler::~PcmResampler() { close(); }

bool PcmResampler::open(int inSampleRate, int inChannels, PcmSampleFormat inFormat,
                        int outSampleRate, int outChannels, PcmSampleFormat outFormat) {
    close();
    if (inSampleRate <= 0 || inChannels <= 0 || outSampleRate <= 0 || outChannels <= 0) {
        return false;
    }

    inRate_ = inSampleRate;
    inChannels_ = inChannels;
    inFormat_ = inFormat;
    outChannels_ = outChannels;
    outFormat_ = outFormat;

    AVChannelLayout inLayout;
    av_channel_layout_default(&inLayout, inChannels);
    AVChannelLayout outLayout;
    av_channel_layout_default(&outLayout, outChannels);

    const int ret = swr_alloc_set_opts2(&swr_, &outLayout, toAvFormat(outFormat), outSampleRate,
                                        &inLayout, toAvFormat(inFormat), inSampleRate, 0,
                                        nullptr);
    av_channel_layout_uninit(&inLayout);
    av_channel_layout_uninit(&outLayout);

    if (ret < 0 || !swr_) {
        if (swr_) swr_free(&swr_);
        return false;
    }
    if (swr_init(swr_) < 0) {
        swr_free(&swr_);
        return false;
    }
    return true;
}

void PcmResampler::close() {
    if (swr_) swr_free(&swr_);
}

QByteArray PcmResampler::convert(const QByteArray& input) {
    if (!swr_ || input.isEmpty()) return {};

    const int frameBytes = bytesPerSampleOf(inFormat_) * inChannels_;
    const int inSamples = input.size() / frameBytes;
    if (inSamples <= 0) return {};

    const int maxOut = swr_get_out_samples(swr_, inSamples);
    if (maxOut <= 0) return {};

    const int outBytesPerSample = bytesPerSampleOf(outFormat_);
    QByteArray out(maxOut * outChannels_ * outBytesPerSample, Qt::Uninitialized);
    uint8_t* outData[1] = { reinterpret_cast<uint8_t*>(out.data()) };
    const uint8_t* inData[1] = { reinterpret_cast<const uint8_t*>(input.constData()) };

    const int got = swr_convert(swr_, outData, maxOut, inData, inSamples);
    if (got <= 0) {
        out.clear();
        return out;
    }
    out.resize(got * outChannels_ * outBytesPerSample);
    return out;
}
