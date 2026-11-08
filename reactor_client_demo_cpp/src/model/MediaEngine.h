#pragma once
// MediaEngine.h - 媒体引擎（MVC 中的 Model 层）
//
// 职责：
//   1. 一个 QUdpSocket 同时收发 RTP（服务端用源地址反查会话，收发必须是同一端口）；
//   2. 摄像头采集 + H.264 编码（独立线程）/ H.264 解码（独立线程）；
//   3. QAudioSource 采集 + AAC 编码 / AAC 解码 + QAudioSink 播放；
//   4. 输出帧与统计信号给控制器，不直接接触任何界面对象。
#include <QByteArray>
#include <QImage>
#include <QObject>
#include <QString>
#include <QStringList>

#include "AudioCodec.h"
#include "RtpPacket.h"

class QUdpSocket;
class QAudioSource;
class QAudioSink;
class QIODevice;
class QTimer;
class CameraThread;
class DecodeThread;
class PcmBuffer;

class MediaEngine : public QObject {
    Q_OBJECT
public:
    explicit MediaEngine(QObject* parent = nullptr);
    ~MediaEngine() override;

    // ---------------- 生命周期 ----------------
    // 绑定本地 UDP 端口（0 表示由系统分配），成功后可取 localPort()
    bool start();
    void stopAll();

    quint16 localPort() const { return localPort_; }

    // 服务器媒体中继地址（reactor_server 默认 10001）
    void setRelayTarget(const QString& host, quint16 port);

    // MEDIA_REGISTER 使用的本地 IP 与当前房间；进入/离开房间后需重新设置
    void setRegisterInfo(const QString& localIp, const QString& roomId);
    QString localIp() const { return localIp_; }
    QString roomId() const { return roomId_; }

    // ---------------- 设备能力 ----------------
    static bool hasAudioInput();
    static bool hasAudioOutput();
    bool videoSending() const { return camera_ != nullptr; }
    bool audioSending() const { return audioSource_ != nullptr; }

    // ---------------- 硬件选择 ----------------
    // 名称列表按系统枚举顺序，摄像头列表下标即 OpenCV 的 capture 索引
    static QStringList cameraDeviceNames();
    static QStringList audioInputDeviceNames();
    static QStringList audioOutputDeviceNames();

    int cameraDeviceIndex() const { return cameraIndex_; }
    int audioInputDeviceIndex() const { return audioInputIndex_; }
    int audioOutputDeviceIndex() const { return audioOutputIndex_; }

    // 选择设备（-1 表示系统默认）；正在采集/播放时立即切换到新设备
    void setCameraDeviceIndex(int index);
    void setAudioInputDeviceIndex(int index);
    void setAudioOutputDeviceIndex(int index);

    // ---------------- 采集控制 ----------------
    bool startVideoSend();
    void stopVideoSend();
    bool startAudioSend();
    void stopAudioSend();
    void setSpeakerMuted(bool muted);

public slots:
    // 供采集/解码线程回调（通过 QMetaObject::invokeMethod 投递回本对象线程）
    void onWorkerPreview(const QImage& image);
    void onWorkerVideoEncoded(const QByteArray& annexb);
    void onWorkerRemoteImage(const QImage& image);
    void onWorkerWarning(const QString& message);

signals:
    void portBound(quint16 port);
    void localFrame(const QImage& image);
    void remoteFrame(const QImage& image);
    void statsChanged(double localFps, int localKbps, double remoteFps, int remoteKbps);
    // 输入/输出音频电平，取值 0.0~1.0（对应 -60dB~0dB）
    void audioLevelsChanged(double inputLevel, double outputLevel);
    void warning(const QString& message);
    void videoSendStateChanged(bool on);
    void audioSendStateChanged(bool on);

private:
    void onReadyRead();
    void sendVideoFrame(const QByteArray& annexb);
    void sendAudioFrame(const QByteArray& aac);
    // notify = false 时不发出停止信号，用于切换设备时避免界面按钮闪烁
    void stopVideoSendInternal(bool notify);
    void stopAudioSendInternal(bool notify);
    void setupAudioOutput();
    void teardownAudioOutput();
    void emitStats();
    void emitAudioLevels();

    QUdpSocket* socket_ = nullptr;
    quint16 localPort_ = 0;

    QString relayHost_;
    quint16 relayPort_ = 10001;
    QString localIp_;
    QString roomId_;

    rtp::Reassembler videoReassembler_;
    rtp::Reassembler audioReassembler_;

    quint16 videoSeq_ = 0;
    quint16 audioSeq_ = 0;
    quint32 videoFrameId_ = 0;
    quint32 audioFrameId_ = 0;
    quint32 videoTimestamp_ = 0;
    quint32 audioTimestamp_ = 0;
    quint32 ssrc_ = 0;

    CameraThread* camera_ = nullptr;
    DecodeThread* decoder_ = nullptr;

    int cameraIndex_ = -1;      // -1 = 系统默认摄像头
    int audioInputIndex_ = -1;  // -1 = 系统默认输入设备
    int audioOutputIndex_ = -1; // -1 = 系统默认输出设备

    AacEncoder audioEncoder_;
    AacDecoder audioDecoder_;
    PcmResampler audioResampler_;   // 采集格式 -> 48k/2ch/S16，格式不一致时启用
    PcmResampler outputResampler_;  // 解码结果 48k/2ch/S16 -> 扬声器格式，格式不一致时启用
    QAudioSource* audioSource_ = nullptr;
    QIODevice* audioInputIo_ = nullptr;
    QTimer* audioCaptureTimer_ = nullptr;

    QAudioSink* audioSink_ = nullptr;
    PcmBuffer* pcmBuffer_ = nullptr;
    bool speakerMuted_ = false;

    QTimer* statsTimer_ = nullptr;
    QTimer* levelTimer_ = nullptr;      // 50ms 刷新一次电平表
    double micLevel_ = 0.0;             // 平滑后的麦克风输入电平
    double micTarget_ = 0.0;            // 本次采样到的峰值（逐周期衰减）
    double speakerLevel_ = 0.0;         // 平滑后的扬声器输出电平
    double speakerTarget_ = 0.0;
    int localVideoFrames_ = 0;
    int localVideoBytes_ = 0;
    int remoteVideoFrames_ = 0;
    int remoteVideoBytes_ = 0;
};
