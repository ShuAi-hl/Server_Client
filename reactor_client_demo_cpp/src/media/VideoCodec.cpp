// VideoCodec.cpp - FFmpeg H.264 编解码实现
#include "VideoCodec.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

#include <cstring>

H264Encoder::~H264Encoder() { close(); }

bool H264Encoder::open(int width, int height, int fps, int bitrate) {
    close();

    const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec) return false;

    width_ = width;
    height_ = height;
    fps_ = fps > 0 ? fps : 30;

    ctx_ = avcodec_alloc_context3(codec);
    if (!ctx_) return false;

    ctx_->width = width;
    ctx_->height = height;
    ctx_->time_base = {1, fps_};
    ctx_->framerate = {fps_, 1};
    ctx_->pix_fmt = AV_PIX_FMT_YUV420P;
    ctx_->bit_rate = bitrate;
    ctx_->gop_size = fps_ * 2;      // 2 秒一个关键帧，便于丢包后恢复
    ctx_->max_b_frames = 0;         // 实时低延迟
    ctx_->thread_count = 2;

    // ultrafast + zerolatency；repeat-headers 让 SPS/PPS 随关键帧在带内下发，
    // 这样解码端无需额外传递 extradata
    av_opt_set(ctx_->priv_data, "preset", "ultrafast", 0);
    av_opt_set(ctx_->priv_data, "tune", "zerolatency", 0);
    av_opt_set(ctx_->priv_data, "x264-params", "repeat-headers=1:scenecut=0", 0);

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
    frame_->format = AV_PIX_FMT_YUV420P;
    frame_->width = width;
    frame_->height = height;
    if (av_frame_get_buffer(frame_, 32) < 0) {
        close();
        return false;
    }

    sws_ = sws_getContext(width, height, AV_PIX_FMT_BGR24,
                          width, height, AV_PIX_FMT_YUV420P,
                          SWS_BILINEAR, nullptr, nullptr, nullptr);
    return sws_ != nullptr;
}

void H264Encoder::close() {
    if (sws_) { sws_freeContext(sws_); sws_ = nullptr; }
    if (frame_) { av_frame_free(&frame_); }
    if (pkt_) { av_packet_free(&pkt_); }
    if (ctx_) { avcodec_free_context(&ctx_); }
    pts_ = 0;
}

bool H264Encoder::convertToYuv(const unsigned char* bgrData, int width, int height, int stride) {
    if (av_frame_make_writable(frame_) < 0) return false;
    const uint8_t* src[1] = { bgrData };
    int srcStride[1] = { stride };
    sws_scale(sws_, src, srcStride, 0, height, frame_->data, frame_->linesize);
    return true;
}

bool H264Encoder::drainPackets(std::vector<QByteArray>& packets) {
    while (true) {
        int ret = avcodec_receive_packet(ctx_, pkt_);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) return true;
        if (ret < 0) return false;

        packets.emplace_back(QByteArray(reinterpret_cast<const char*>(pkt_->data), pkt_->size));
        av_packet_unref(pkt_);
    }
}

bool H264Encoder::encode(const unsigned char* bgrData, int width, int height, int stride,
                         std::vector<QByteArray>& packets, QString& err) {
    if (!isOpen()) {
        err = QStringLiteral("编码器未打开");
        return false;
    }
    // 摄像头实际分辨率与编码尺寸不一致时先缩放到目标尺寸
    if (width != width_ || height != height_) {
        err = QStringLiteral("输入尺寸与编码尺寸不一致");
        return false;
    }
    if (!convertToYuv(bgrData, width, height, stride)) {
        err = QStringLiteral("像素格式转换失败");
        return false;
    }
    frame_->pts = pts_++;
    frame_->pict_type = AV_PICTURE_TYPE_NONE;

    int ret = avcodec_send_frame(ctx_, frame_);
    if (ret < 0) {
        err = QStringLiteral("编码 send_frame 失败：%1").arg(ret);
        return false;
    }
    if (!drainPackets(packets)) {
        err = QStringLiteral("编码 receive_packet 失败");
        return false;
    }
    return true;
}

// ============================ 解码器 ============================

H264Decoder::~H264Decoder() { close(); }

bool H264Decoder::open() {
    close();

    const AVCodec* codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!codec) return false;

    ctx_ = avcodec_alloc_context3(codec);
    if (!ctx_) return false;
    ctx_->thread_count = 2;
    // 网络丢包后尽量容错，不要直接报错中断
    ctx_->flags |= AV_CODEC_FLAG_OUTPUT_CORRUPT;
    ctx_->err_recognition = 0;

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

void H264Decoder::close() {
    if (sws_) { sws_freeContext(sws_); sws_ = nullptr; }
    swsW_ = swsH_ = 0;
    if (frame_) { av_frame_free(&frame_); }
    if (pkt_) { av_packet_free(&pkt_); }
    if (ctx_) { avcodec_free_context(&ctx_); }
}

bool H264Decoder::decode(const QByteArray& annexb, QImage& image, QString& err) {
    if (!isOpen()) {
        err = QStringLiteral("解码器未打开");
        return false;
    }
    if (annexb.isEmpty()) return false;

    av_packet_unref(pkt_);
    if (av_new_packet(pkt_, annexb.size()) < 0) {
        err = QStringLiteral("分配解码包失败");
        return false;
    }
    std::memcpy(pkt_->data, annexb.constData(), static_cast<size_t>(annexb.size()));

    int ret = avcodec_send_packet(ctx_, pkt_);
    if (ret < 0) {
        // 参考帧缺失等错误：丢弃当前帧继续解码
        return false;
    }

    ret = avcodec_receive_frame(ctx_, frame_);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) return false;
    if (ret < 0) return false;

    const int w = frame_->width;
    const int h = frame_->height;
    if (w <= 0 || h <= 0) return false;

    if (!sws_ || w != swsW_ || h != swsH_) {
        if (sws_) sws_freeContext(sws_);
        sws_ = sws_getContext(w, h, static_cast<AVPixelFormat>(frame_->format),
                              w, h, AV_PIX_FMT_BGR24,
                              SWS_BILINEAR, nullptr, nullptr, nullptr);
        swsW_ = w;
        swsH_ = h;
    }
    if (!sws_) {
        err = QStringLiteral("解码像素转换上下文创建失败");
        return false;
    }

    image = QImage(w, h, QImage::Format_BGR888);
    uint8_t* dst[1] = { reinterpret_cast<uint8_t*>(image.bits()) };
    int dstStride[1] = { static_cast<int>(image.bytesPerLine()) };
    sws_scale(sws_, frame_->data, frame_->linesize, 0, h, dst, dstStride);
    return true;
}
