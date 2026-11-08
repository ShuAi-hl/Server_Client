// MediaEngine.cpp - 媒体引擎实现
#include "MediaEngine.h"

#include <QAudioDevice>
#include <QAudioFormat>
#include <QAudioSink>
#include <QAudioSource>
#include <QCameraDevice>
#include <QElapsedTimer>
#include <QHostAddress>
#include <QMediaDevices>
#include <QMutex>
#include <QQueue>
#include <cmath>
#include <QThread>
#include <QTimer>
#include <QUdpSocket>
#include <QWaitCondition>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include <cstring>

#include "VideoCodec.h"

namespace {
constexpr int kVideoWidth = 640;
constexpr int kVideoHeight = 480;
constexpr int kVideoFps = 30;
constexpr int kVideoBitrate = 900000;

constexpr int kAudioSampleRate = 48000;
constexpr int kAudioChannels = 2;
constexpr int kAudioBitrate = 128000;

// 音频播放缓冲上限：约 0.5 秒，超出丢弃最旧数据以保持实时
constexpr int kMaxPcmBufferBytes = kAudioSampleRate * kAudioChannels * 2 / 2;

// 计算 S16 交错 PCM 的音量电平，返回 0.0~1.0（对应 -60dB~0dB）
double pcmLevel(const QByteArray& pcm) {
    const int samples = pcm.size() / static_cast<int>(sizeof(qint16));
    if (samples <= 0) return 0.0;

    const qint16* data = reinterpret_cast<const qint16*>(pcm.constData());
    double sum = 0.0;
    for (int i = 0; i < samples; ++i) {
        const double v = static_cast<double>(data[i]) / 32768.0;
        sum += v * v;
    }
    const double rms = std::sqrt(sum / samples);
    if (rms <= 1e-5) return 0.0;

    const double db = 20.0 * std::log10(rms);  // -inf ~ 0
    return qBound(0.0, (db + 60.0) / 60.0, 1.0);
}
}  // namespace

// ======================== 音频播放缓冲（pull 模式） ========================

class PcmBuffer : public QIODevice {
public:
    explicit PcmBuffer(QObject* parent = nullptr) : QIODevice(parent) {
        open(QIODevice::ReadOnly);
    }

    void append(const QByteArray& pcm) {
        QMutexLocker locker(&mutex_);
        buffer_.append(pcm);
        if (buffer_.size() > kMaxPcmBufferBytes) {
            buffer_.remove(0, buffer_.size() - kMaxPcmBufferBytes);
        }
    }

    void clearBuffer() {
        QMutexLocker locker(&mutex_);
        buffer_.clear();
    }

protected:
    qint64 readData(char* data, qint64 maxSize) override {
        QMutexLocker locker(&mutex_);
        if (buffer_.isEmpty() || maxSize <= 0) return 0;
        const qint64 n = qMin(maxSize, static_cast<qint64>(buffer_.size()));
        std::memcpy(data, buffer_.constData(), static_cast<size_t>(n));
        buffer_.remove(0, static_cast<int>(n));
        return n;
    }

    qint64 writeData(const char*, qint64) override { return -1; }

    qint64 bytesAvailable() const override {
        QMutexLocker locker(&mutex_);
        return buffer_.size() + QIODevice::bytesAvailable();
    }

private:
    mutable QMutex mutex_;
    QByteArray buffer_;
};

// ======================== 摄像头采集 + 编码线程 ========================

class CameraThread : public QThread {
public:
    CameraThread(MediaEngine* engine, int deviceIndex, QObject* parent = nullptr)
        : QThread(parent), engine_(engine), deviceIndex_(deviceIndex) {}

protected:
    void run() override;
    void warn(const QString& text);

private:
    MediaEngine* engine_;
    int deviceIndex_ = -1;
};

void CameraThread::warn(const QString& text) {
    QMetaObject::invokeMethod(engine_, [e = engine_, text]() { e->onWorkerWarning(text); },
                              Qt::QueuedConnection);
}

void CameraThread::run() {
    cv::VideoCapture capture(deviceIndex_ < 0 ? 0 : deviceIndex_);
    if (!capture.isOpened()) {
        warn(QStringLiteral("无法打开摄像头 #%1（设备被占用或不存在）")
                 .arg(deviceIndex_ < 0 ? 0 : deviceIndex_));
        return;
    }
    capture.set(cv::CAP_PROP_FRAME_WIDTH, kVideoWidth);
    capture.set(cv::CAP_PROP_FRAME_HEIGHT, kVideoHeight);
    capture.set(cv::CAP_PROP_FPS, kVideoFps);

    H264Encoder encoder;
    if (!encoder.open(kVideoWidth, kVideoHeight, kVideoFps, kVideoBitrate)) {
        warn(QStringLiteral("H.264 编码器初始化失败"));
        return;
    }

    QElapsedTimer frameTimer;
    frameTimer.start();

    while (!isInterruptionRequested()) {
        cv::Mat source;
        capture >> source;
        if (source.empty()) {
            msleep(20);
            continue;
        }

        // 摄像头分辨率与编码尺寸不一致时缩放到固定尺寸
        cv::Mat bgr = source;
        if (source.cols != kVideoWidth || source.rows != kVideoHeight) {
            cv::resize(source, bgr, cv::Size(kVideoWidth, kVideoHeight));
        }

        std::vector<QByteArray> packets;
        QString err;
        if (encoder.encode(bgr.data, bgr.cols, bgr.rows, static_cast<int>(bgr.step),
                           packets, err)) {
            QImage preview(bgr.data, bgr.cols, bgr.rows,
                           static_cast<int>(bgr.step), QImage::Format_BGR888);
            const QImage copy = preview.copy();

            QByteArray annexb;
            for (const QByteArray& p : packets) annexb.append(p);

            QMetaObject::invokeMethod(engine_, [e = engine_, copy]() {
                e->onWorkerPreview(copy);
            }, Qt::QueuedConnection);

            if (!annexb.isEmpty()) {
                QMetaObject::invokeMethod(engine_, [e = engine_, annexb]() {
                    e->onWorkerVideoEncoded(annexb);
                }, Qt::QueuedConnection);
            }
        }

        // 固定 30fps 节流
        const int elapsed = static_cast<int>(frameTimer.restart());
        const int wait = 1000 / kVideoFps - elapsed;
        if (wait > 0) msleep(static_cast<unsigned long>(wait));
    }
}

// ======================== H.264 解码线程 ========================

class DecodeThread : public QThread {
public:
    explicit DecodeThread(MediaEngine* engine, QObject* parent = nullptr)
        : QThread(parent), engine_(engine) {}

    // 只保留最新两帧，解码跟不上时丢弃旧帧保证实时性
    void pushFrame(const QByteArray& frame) {
        QMutexLocker locker(&mutex_);
        if (queue_.size() >= 2) queue_.removeFirst();
        queue_.append(frame);
        waitCondition_.wakeOne();
    }

    void stop() {
        {
            QMutexLocker locker(&mutex_);
            stopped_ = true;
            queue_.clear();
        }
        waitCondition_.wakeAll();
    }

protected:
    void run() override {
        H264Decoder decoder;
        if (!decoder.open()) {
            QMetaObject::invokeMethod(engine_, [e = engine_]() {
                e->onWorkerWarning(QStringLiteral("H.264 解码器初始化失败"));
            }, Qt::QueuedConnection);
            return;
        }

        while (true) {
            QByteArray frame;
            {
                QMutexLocker locker(&mutex_);
                while (queue_.isEmpty() && !stopped_) {
                    waitCondition_.wait(&mutex_);
                }
                if (stopped_) break;
                frame = queue_.takeFirst();
            }

            QImage image;
            QString err;
            if (decoder.decode(frame, image, err)) {
                QMetaObject::invokeMethod(engine_, [e = engine_, image]() {
                    e->onWorkerRemoteImage(image);
                }, Qt::QueuedConnection);
            }
        }
    }

private:
    MediaEngine* engine_;
    QMutex mutex_;
    QWaitCondition waitCondition_;
    QQueue<QByteArray> queue_;
    bool stopped_ = false;
};

// ======================== MediaEngine ========================

MediaEngine::MediaEngine(QObject* parent) : QObject(parent) {
    ssrc_ = rtp::makeSsrc();
}

MediaEngine::~MediaEngine() { stopAll(); }

bool MediaEngine::start() {
    if (socket_) return true;

    socket_ = new QUdpSocket(this);
    // 绑定 AnyIPv4 的随机端口：服务端按「源地址」反查会话，收发必须同一个 socket
    if (!socket_->bind(QHostAddress::AnyIPv4, 0)) {
        emit warning(QStringLiteral("绑定本地 UDP 端口失败：%1").arg(socket_->errorString()));
        delete socket_;
        socket_ = nullptr;
        return false;
    }
    localPort_ = socket_->localPort();
    connect(socket_, &QUdpSocket::readyRead, this, &MediaEngine::onReadyRead);

    audioDecoder_.open();
    decoder_ = new DecodeThread(this);
    decoder_->start();

    statsTimer_ = new QTimer(this);
    statsTimer_->setInterval(1000);
    connect(statsTimer_, &QTimer::timeout, this, &MediaEngine::emitStats);
    statsTimer_->start();

    // 电平表 20Hz 刷新：平滑上升 + 快速回落，静音时归零
    levelTimer_ = new QTimer(this);
    levelTimer_->setInterval(50);
    connect(levelTimer_, &QTimer::timeout, this, &MediaEngine::emitAudioLevels);
    levelTimer_->start();

    setupAudioOutput();

    emit portBound(localPort_);
    return true;
}

void MediaEngine::stopAll() {
    stopVideoSend();
    stopAudioSend();

    if (decoder_) {
        decoder_->stop();
        decoder_->wait(2000);
        delete decoder_;
        decoder_ = nullptr;
    }
    teardownAudioOutput();
    audioDecoder_.close();

    if (statsTimer_) {
        statsTimer_->stop();
        statsTimer_->deleteLater();
        statsTimer_ = nullptr;
    }
    if (levelTimer_) {
        levelTimer_->stop();
        levelTimer_->deleteLater();
        levelTimer_ = nullptr;
    }
    micLevel_ = micTarget_ = 0.0;
    speakerLevel_ = speakerTarget_ = 0.0;
    emit audioLevelsChanged(0.0, 0.0);
    if (socket_) {
        socket_->close();
        socket_->deleteLater();
        socket_ = nullptr;
        localPort_ = 0;
    }
}

void MediaEngine::setRelayTarget(const QString& host, quint16 port) {
    relayHost_ = host;
    relayPort_ = port;
}

void MediaEngine::setRegisterInfo(const QString& localIp, const QString& roomId) {
    localIp_ = localIp;
    roomId_ = roomId;
}

bool MediaEngine::hasAudioInput() {
    return !QMediaDevices::audioInputs().isEmpty();
}

QStringList MediaEngine::cameraDeviceNames() {
    QStringList names;
    for (const QCameraDevice& device : QMediaDevices::videoInputs()) {
        names << (device.description().isEmpty() ? device.id() : device.description());
    }
    return names;
}

QStringList MediaEngine::audioInputDeviceNames() {
    QStringList names;
    for (const QAudioDevice& device : QMediaDevices::audioInputs()) {
        names << (device.description().isEmpty() ? device.id() : device.description());
    }
    return names;
}

QStringList MediaEngine::audioOutputDeviceNames() {
    QStringList names;
    for (const QAudioDevice& device : QMediaDevices::audioOutputs()) {
        names << (device.description().isEmpty() ? device.id() : device.description());
    }
    return names;
}

void MediaEngine::setCameraDeviceIndex(int index) {
    if (index < -1 || cameraIndex_ == index) return;
    cameraIndex_ = index;

    // 正在采集时静默重启线程，避免界面按钮出现"关闭→开启"的闪烁
    if (camera_) {
        stopVideoSendInternal(false);
        startVideoSend();
    }
}

void MediaEngine::setAudioInputDeviceIndex(int index) {
    if (index < -1 || audioInputIndex_ == index) return;
    audioInputIndex_ = index;

    if (audioSource_) {
        stopAudioSendInternal(false);
        if (!startAudioSend()) emit audioSendStateChanged(false);
    }
}

void MediaEngine::setAudioOutputDeviceIndex(int index) {
    if (index < -1 || audioOutputIndex_ == index) return;
    audioOutputIndex_ = index;

    // 正在播放时静默重建 sink：保留 pcmBuffer_ 里已缓存的数据，切换后不会断音
    if (audioSink_) {
        audioSink_->stop();
        audioSink_->deleteLater();
        audioSink_ = nullptr;
        setupAudioOutput();
    }
}

bool MediaEngine::hasAudioOutput() {
    return !QMediaDevices::audioOutputs().isEmpty();
}

// ---------------- 视频发送 ----------------

bool MediaEngine::startVideoSend() {
    if (camera_) return true;
    camera_ = new CameraThread(this, cameraIndex_);
    camera_->start();
    emit videoSendStateChanged(true);
    return true;
}

void MediaEngine::stopVideoSend() { stopVideoSendInternal(true); }

void MediaEngine::stopVideoSendInternal(bool notify) {
    if (!camera_) return;
    camera_->requestInterruption();
    camera_->wait(2000);
    delete camera_;
    camera_ = nullptr;
    if (notify) emit videoSendStateChanged(false);
}

void MediaEngine::onWorkerPreview(const QImage& image) {
    if (image.isNull()) return;
    emit localFrame(image);
}

void MediaEngine::onWorkerVideoEncoded(const QByteArray& annexb) {
    sendVideoFrame(annexb);
}

void MediaEngine::onWorkerRemoteImage(const QImage& image) {
    if (image.isNull()) return;
    remoteVideoFrames_++;
    emit remoteFrame(image);
}

void MediaEngine::onWorkerWarning(const QString& message) {
    emit warning(message);
}

// ---------------- 音频发送 ----------------

namespace {

// QAudioFormat 采样格式 -> 重采样器采样格式
bool mapSampleFormat(QAudioFormat::SampleFormat src, PcmSampleFormat& dst) {
    switch (src) {
        case QAudioFormat::UInt8: dst = PcmSampleFormat::U8; return true;
        case QAudioFormat::Int16: dst = PcmSampleFormat::S16; return true;
        case QAudioFormat::Int32: dst = PcmSampleFormat::S32; return true;
        case QAudioFormat::Float: dst = PcmSampleFormat::F32; return true;
        default: return false;
    }
}

// 优先选择 48kHz 双声道/单声道 S16，其次 44.1kHz/32kHz，最后回退到设备首选格式。
// 之前的实现只接受 48kHz/双声道/S16，很多笔记本/USB 麦克风（仅单声道或 44.1kHz）
// 因此被直接拒绝，表现为"麦克风打开失败"。
QAudioFormat chooseInputFormat(const QAudioDevice& device) {
    const int rates[] = {48000, 44100, 32000};
    const int channels[] = {2, 1};
    for (int rate : rates) {
        for (int ch : channels) {
            QAudioFormat f;
            f.setSampleRate(rate);
            f.setChannelCount(ch);
            f.setSampleFormat(QAudioFormat::Int16);
            if (device.isFormatSupported(f)) return f;
        }
    }
    return device.preferredFormat();
}

}  // namespace

bool MediaEngine::startAudioSend() {
    if (audioSource_) return true;

    // 指定了设备序号则用该设备，否则用系统默认（-1）
    const QList<QAudioDevice> inputs = QMediaDevices::audioInputs();
    QAudioDevice device;
    if (audioInputIndex_ >= 0 && audioInputIndex_ < inputs.size()) {
        device = inputs.at(audioInputIndex_);
    } else if (!inputs.isEmpty()) {
        device = QMediaDevices::defaultAudioInput().isNull() ? inputs.first()
                                                             : QMediaDevices::defaultAudioInput();
    }
    if (device.isNull()) {
        emit warning(QStringLiteral("未检测到可用的音频输入设备，麦克风功能不可用"));
        return false;
    }

    // 与设备协商实际可用的采集格式，不再强制要求 48kHz/双声道/S16
    const QAudioFormat format = chooseInputFormat(device);
    PcmSampleFormat pcmFormat = PcmSampleFormat::S16;
    if (format.sampleRate() <= 0 || format.channelCount() <= 0
        || !mapSampleFormat(format.sampleFormat(), pcmFormat)) {
        emit warning(QStringLiteral("音频输入设备「%1」没有可用的采集格式，麦克风不可用")
                         .arg(device.description()));
        return false;
    }

    // 设备格式与编码器所需格式不一致时，采集后先重采样到 48kHz/双声道/S16
    const bool needResample = format.sampleRate() != kAudioSampleRate
                              || format.channelCount() != kAudioChannels
                              || pcmFormat != PcmSampleFormat::S16;
    if (needResample
        && !audioResampler_.open(format.sampleRate(), format.channelCount(), pcmFormat)) {
        emit warning(QStringLiteral("音频重采样初始化失败（%1Hz / %2 声道）")
                         .arg(format.sampleRate())
                         .arg(format.channelCount()));
        return false;
    }

    if (!audioEncoder_.open(kAudioSampleRate, kAudioChannels, kAudioBitrate)) {
        emit warning(QStringLiteral("AAC 编码器初始化失败"));
        audioResampler_.close();
        return false;
    }

    audioSource_ = new QAudioSource(device, format, this);
    audioInputIo_ = audioSource_->start();
    if (!audioInputIo_) {
        emit warning(QStringLiteral("打开音频输入设备「%1」失败（设备可能被占用）")
                         .arg(device.description()));
        audioSource_->deleteLater();
        audioSource_ = nullptr;
        audioEncoder_.close();
        audioResampler_.close();
        return false;
    }

    // 20ms 拉取一次 PCM（960 采样），交给 AAC 编码器凑满 1024 采样一帧
    audioCaptureTimer_ = new QTimer(this);
    audioCaptureTimer_->setInterval(20);
    connect(audioCaptureTimer_, &QTimer::timeout, this, [this]() {
        if (!audioInputIo_) return;
        QByteArray pcm = audioInputIo_->readAll();
        if (pcm.isEmpty()) return;
        micTarget_ = qMax(micTarget_, pcmLevel(pcm));
        if (audioResampler_.isOpen()) pcm = audioResampler_.convert(pcm);
        if (pcm.isEmpty()) return;
        std::vector<QByteArray> frames;
        audioEncoder_.feed(pcm, frames);
        for (const QByteArray& frame : frames) sendAudioFrame(frame);
    });
    audioCaptureTimer_->start();

    emit audioSendStateChanged(true);
    return true;
}

void MediaEngine::stopAudioSend() { stopAudioSendInternal(true); }

void MediaEngine::stopAudioSendInternal(bool notify) {
    if (audioCaptureTimer_) {
        audioCaptureTimer_->stop();
        audioCaptureTimer_->deleteLater();
        audioCaptureTimer_ = nullptr;
    }
    if (audioSource_) {
        audioSource_->stop();
        audioSource_->deleteLater();
        audioSource_ = nullptr;
    }
    audioInputIo_ = nullptr;

    if (audioEncoder_.isOpen()) {
        std::vector<QByteArray> frames;
        audioEncoder_.flush(frames);
        for (const QByteArray& frame : frames) sendAudioFrame(frame);
        audioEncoder_.close();
    }
    audioResampler_.close();
    micTarget_ = 0.0;  // 关闭麦克风后电平表回落到静音
    if (notify) emit audioSendStateChanged(false);
}

void MediaEngine::setupAudioOutput() {
    if (audioSink_) return;

    // 指定了设备序号则用该设备，否则用系统默认（-1）
    const QList<QAudioDevice> outputs = QMediaDevices::audioOutputs();
    QAudioDevice device;
    if (audioOutputIndex_ >= 0 && audioOutputIndex_ < outputs.size()) {
        device = outputs.at(audioOutputIndex_);
    } else if (!outputs.isEmpty()) {
        device = QMediaDevices::defaultAudioOutput().isNull() ? outputs.first()
                                                              : QMediaDevices::defaultAudioOutput();
    }
    if (device.isNull()) {
        emit warning(QStringLiteral("未检测到音频输出设备，远端声音将无法播放"));
        return;
    }

    // 优先 48kHz/双声道/S16；设备不支持时退回其首选格式，解码结果再重采样到该格式
    QAudioFormat format;
    format.setSampleRate(kAudioSampleRate);
    format.setChannelCount(kAudioChannels);
    format.setSampleFormat(QAudioFormat::Int16);
    if (!device.isFormatSupported(format)) format = device.preferredFormat();

    PcmSampleFormat pcmFormat = PcmSampleFormat::S16;
    if (format.sampleRate() <= 0 || format.channelCount() <= 0
        || !mapSampleFormat(format.sampleFormat(), pcmFormat)) {
        emit warning(QStringLiteral("音频输出设备「%1」没有可用的播放格式").arg(device.description()));
        return;
    }

    const bool needResample = format.sampleRate() != kAudioSampleRate
                              || format.channelCount() != kAudioChannels
                              || pcmFormat != PcmSampleFormat::S16;
    if (needResample
        && !outputResampler_.open(kAudioSampleRate, kAudioChannels, PcmSampleFormat::S16,
                                  format.sampleRate(), format.channelCount(), pcmFormat)) {
        emit warning(QStringLiteral("音频输出重采样初始化失败（%1Hz / %2 声道）")
                         .arg(format.sampleRate())
                         .arg(format.channelCount()));
        return;
    }

    audioSink_ = new QAudioSink(device, format, this);
    audioSink_->setVolume(speakerMuted_ ? 0.0f : 1.0f);
    if (!pcmBuffer_) pcmBuffer_ = new PcmBuffer(this);
    audioSink_->start(pcmBuffer_);  // pull 模式：由 sink 从缓冲中取数据
}

void MediaEngine::teardownAudioOutput() {
    if (audioSink_) {
        audioSink_->stop();
        audioSink_->deleteLater();
        audioSink_ = nullptr;
    }
    outputResampler_.close();
    speakerTarget_ = 0.0;
    if (pcmBuffer_) {
        pcmBuffer_->deleteLater();
        pcmBuffer_ = nullptr;
    }
}

void MediaEngine::setSpeakerMuted(bool muted) {
    speakerMuted_ = muted;
    if (audioSink_) audioSink_->setVolume(muted ? 0.0f : 1.0f);
}

// ---------------- RTP 发送 ----------------

void MediaEngine::sendVideoFrame(const QByteArray& annexb) {
    if (!socket_ || relayHost_.isEmpty() || annexb.isEmpty()) return;

    const quint32 frameId = ++videoFrameId_;
    const quint32 frameSize = static_cast<quint32>(annexb.size());
    const int total = static_cast<int>((annexb.size() + rtp::kMaxPayloadSize - 1) /
                                       rtp::kMaxPayloadSize);

    for (int i = 0; i < total; ++i) {
        const quint32 offset = static_cast<quint32>(i) * rtp::kMaxPayloadSize;
        const int len = qMin(rtp::kMaxPayloadSize,
                             static_cast<int>(annexb.size()) - static_cast<int>(offset));

        rtp::Header header;
        header.payloadType = rtp::kPayloadTypeVideo;
        header.marker = (i == total - 1);
        header.sequence = videoSeq_++;
        header.timestamp = videoTimestamp_;
        header.ssrc = ssrc_;
        header.frameId = frameId;
        header.offset = offset;
        header.frameSize = frameSize;

        const QByteArray packet = rtp::buildPacket(header, annexb.constData() + offset, len);
        socket_->writeDatagram(packet, QHostAddress(relayHost_), relayPort_);
    }

    videoTimestamp_ += 90000 / kVideoFps;
    localVideoFrames_++;
    localVideoBytes_ += annexb.size();
}

void MediaEngine::sendAudioFrame(const QByteArray& aac) {
    if (!socket_ || relayHost_.isEmpty() || aac.isEmpty()) return;

    const quint32 frameId = ++audioFrameId_;
    const quint32 frameSize = static_cast<quint32>(aac.size());
    const int total = static_cast<int>((aac.size() + rtp::kMaxPayloadSize - 1) /
                                       rtp::kMaxPayloadSize);

    for (int i = 0; i < total; ++i) {
        const quint32 offset = static_cast<quint32>(i) * rtp::kMaxPayloadSize;
        const int len = qMin(rtp::kMaxPayloadSize,
                             static_cast<int>(aac.size()) - static_cast<int>(offset));

        rtp::Header header;
        header.payloadType = rtp::kPayloadTypeAudio;
        header.marker = (i == total - 1);
        header.sequence = audioSeq_++;
        header.timestamp = audioTimestamp_;
        header.ssrc = ssrc_;
        header.frameId = frameId;
        header.offset = offset;
        header.frameSize = frameSize;

        const QByteArray packet = rtp::buildPacket(header, aac.constData() + offset, len);
        socket_->writeDatagram(packet, QHostAddress(relayHost_), relayPort_);
    }

    // 1024 采样 @48kHz 对应一个音频帧时间戳
    audioTimestamp_ += 1024;
}

// ---------------- RTP 接收 -------------------

void MediaEngine::onReadyRead() {
    if (!socket_) return;

    while (socket_->hasPendingDatagrams()) {
        const int size = static_cast<int>(socket_->pendingDatagramSize());
        if (size <= 0) {
            socket_->readDatagram(nullptr, 0);
            continue;
        }

        QByteArray datagram;
        datagram.resize(size);
        socket_->readDatagram(datagram.data(), size);

        rtp::Header header;
        if (!rtp::parsePacket(datagram, header)) continue;

        const QByteArray payload = datagram.mid(rtp::kHeaderSize);

        if (header.payloadType == rtp::kPayloadTypeVideo) {
            QByteArray frame;
            if (videoReassembler_.push(header, payload, frame)) {
                remoteVideoBytes_ += frame.size();
                if (decoder_) decoder_->pushFrame(frame);
            }
        } else if (header.payloadType == rtp::kPayloadTypeAudio) {
            QByteArray frame;
            if (audioReassembler_.push(header, payload, frame)) {
                QByteArray pcm;
                QString err;
                if (audioDecoder_.isOpen() && audioDecoder_.decode(frame, pcm, err) && pcmBuffer_) {
                    speakerTarget_ = qMax(speakerTarget_, pcmLevel(pcm));
                    // 扬声器格式与解码格式(48k/2ch/S16)不一致时先重采样
                    if (outputResampler_.isOpen()) pcm = outputResampler_.convert(pcm);
                    if (!pcm.isEmpty()) pcmBuffer_->append(pcm);
                }
            }
        }
    }
}

void MediaEngine::emitStats() {
    const double localFps = static_cast<double>(localVideoFrames_);
    const int localKbps = localVideoBytes_ * 8 / 1000;
    const double remoteFps = static_cast<double>(remoteVideoFrames_);
    const int remoteKbps = remoteVideoBytes_ * 8 / 1000;

    localVideoFrames_ = 0;
    localVideoBytes_ = 0;
    remoteVideoFrames_ = 0;
    remoteVideoBytes_ = 0;

    emit statsChanged(localFps, localKbps, remoteFps, remoteKbps);
}

void MediaEngine::emitAudioLevels() {
    // 上升快（0.5）、回落稍慢（0.25），峰值逐周期衰减，避免电平条抖动
    micLevel_ += (micTarget_ - micLevel_) * (micTarget_ > micLevel_ ? 0.5 : 0.25);
    speakerLevel_ += (speakerTarget_ - speakerLevel_) * (speakerTarget_ > speakerLevel_ ? 0.5 : 0.25);
    micTarget_ *= 0.6;
    speakerTarget_ *= 0.6;

    if (micLevel_ < 0.005) micLevel_ = 0.0;
    if (speakerLevel_ < 0.005) speakerLevel_ = 0.0;

    emit audioLevelsChanged(micLevel_, speakerLevel_);
}
