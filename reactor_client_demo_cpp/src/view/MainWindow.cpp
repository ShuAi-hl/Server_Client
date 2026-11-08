// MainWindow.cpp - 主窗口实现
#include "MainWindow.h"

#include <QComboBox>
#include <QDateTime>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QScrollBar>
#include <QShortcut>
#include <QSignalBlocker>
#include <QStatusBar>
#include <QStyle>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QWidget>

#include "AppModel.h"
#include "LoginDialog.h"
#include "VideoWidget.h"
#include "theme.h"

MainWindow::MainWindow(AppModel* model, QWidget* parent) : QMainWindow(parent), model_(model) {
    buildUi();
    buildStatusBar();
    connectModelSignals();

    // 初始状态
    onConnStateChanged();
    onRoomsChanged();
    onRoomChanged();
    onMembersChanged();
    onChatChanged();
    onMediaStateChanged();
    onMediaStatsChanged();
    onStatusTextChanged(model_->statusText());
}

void MainWindow::buildUi() {
    setWindowTitle(QStringLiteral("Reactor 会议客户端"));
    resize(1280, 820);
    setMinimumSize(1100, 680);

    QWidget* central = new QWidget(this);
    QVBoxLayout* root = new QVBoxLayout(central);
    root->setContentsMargins(16, 16, 16, 12);
    root->setSpacing(14);

    // ---------------- 顶部标题与连接操作 ----------------
    QHBoxLayout* header = new QHBoxLayout();
    header->setSpacing(12);

    QLabel* title = new QLabel(QStringLiteral("会议客户端"), central);
    title->setObjectName("windowTitle");
    QLabel* subtitle = new QLabel(QStringLiteral("TCP 信令 · UDP 媒体中继 · H.264 + AAC"), central);
    subtitle->setObjectName("caption");

    connectButton_ = new QPushButton(QStringLiteral("连接设置"), central);
    disconnectButton_ = new QPushButton(QStringLiteral("断开"), central);
    disconnectButton_->setProperty("variant", "danger");

    header->addWidget(title);
    header->addWidget(subtitle);
    header->addStretch();
    header->addWidget(connectButton_);
    header->addWidget(disconnectButton_);
    root->addLayout(header);

    // ---------------- 三栏内容 ----------------
    QHBoxLayout* content = new QHBoxLayout();
    content->setSpacing(14);

    // ---- 左：房间列表 ----
    QWidget* left = new QWidget(central);
    left->setObjectName("panel");
    left->setFixedWidth(286);
    QVBoxLayout* leftLayout = new QVBoxLayout(left);
    leftLayout->setContentsMargins(14, 14, 14, 14);
    leftLayout->setSpacing(10);

    QLabel* roomsTitle = new QLabel(QStringLiteral("房间"), left);
    roomsTitle->setObjectName("sectionTitle");
    leftLayout->addWidget(roomsTitle);

    roomList_ = new QListWidget(left);
    roomList_->setSelectionMode(QAbstractItemView::SingleSelection);
    leftLayout->addWidget(roomList_, 1);

    QHBoxLayout* roomRow1 = new QHBoxLayout();
    roomRow1->setSpacing(8);
    refreshButton_ = new QPushButton(QStringLiteral("刷新"), left);
    createButton_ = new QPushButton(QStringLiteral("创建房间"), left);
    createButton_->setProperty("variant", "primary");
    roomRow1->addWidget(refreshButton_);
    roomRow1->addWidget(createButton_);

    QHBoxLayout* roomRow2 = new QHBoxLayout();
    roomRow2->setSpacing(8);
    joinButton_ = new QPushButton(QStringLiteral("加入"), left);
    leaveButton_ = new QPushButton(QStringLiteral("离开"), left);
    leaveButton_->setProperty("variant", "danger");
    roomRow2->addWidget(joinButton_);
    roomRow2->addWidget(leaveButton_);

    leftLayout->addLayout(roomRow1);
    leftLayout->addLayout(roomRow2);

    // ---- 中：视频画面 ----
    QWidget* center = new QWidget(central);
    center->setObjectName("panel");
    center->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QVBoxLayout* centerLayout = new QVBoxLayout(center);
    centerLayout->setContentsMargins(14, 14, 14, 14);
    centerLayout->setSpacing(12);

    localView_ = new VideoWidget(QStringLiteral("本地画面"), center);
    remoteView_ = new VideoWidget(QStringLiteral("远端画面"), center);
    remoteView_->setPlaceholderText(QStringLiteral("等待远端画面"));
    centerLayout->addWidget(localView_, 1);
    centerLayout->addWidget(remoteView_, 1);

    // ---- 硬件设备选择：有多个设备时在此切换（第一行 摄像头/麦克风，第二行 扬声器）----
    QVBoxLayout* deviceArea = new QVBoxLayout();
    deviceArea->setSpacing(6);

    QHBoxLayout* deviceBar = new QHBoxLayout();
    deviceBar->setSpacing(8);

    QLabel* cameraLabel = new QLabel(QStringLiteral("摄像头"), center);
    cameraLabel->setObjectName("caption");
    cameraCombo_ = new QComboBox(center);
    cameraCombo_->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    cameraCombo_->setMinimumContentsLength(6);
    cameraCombo_->setToolTip(QStringLiteral("选择用于采集并发送的摄像头，切换后立即生效"));

    QLabel* micLabel = new QLabel(QStringLiteral("麦克风"), center);
    micLabel->setObjectName("caption");
    micCombo_ = new QComboBox(center);
    micCombo_->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    micCombo_->setMinimumContentsLength(6);
    micCombo_->setToolTip(QStringLiteral("选择用于采集并发送的麦克风，切换后立即生效"));

    deviceBar->addWidget(cameraLabel);
    deviceBar->addWidget(cameraCombo_, 1);
    deviceBar->addSpacing(6);
    deviceBar->addWidget(micLabel);
    deviceBar->addWidget(micCombo_, 1);
    deviceArea->addLayout(deviceBar);

    QHBoxLayout* speakerBar = new QHBoxLayout();
    speakerBar->setSpacing(8);

    QLabel* speakerLabel = new QLabel(QStringLiteral("扬声器"), center);
    speakerLabel->setObjectName("caption");
    speakerCombo_ = new QComboBox(center);
    speakerCombo_->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    speakerCombo_->setMinimumContentsLength(6);
    speakerCombo_->setToolTip(QStringLiteral("选择用于播放远端语音的扬声器，切换后立即生效"));

    QLabel* speakerHint = new QLabel(QStringLiteral("用于播放房间内其他人的声音"), center);
    speakerHint->setObjectName("caption");

    speakerBar->addWidget(speakerLabel);
    speakerBar->addWidget(speakerCombo_, 1);
    speakerBar->addSpacing(6);
    speakerBar->addWidget(speakerHint, 0);
    deviceArea->addLayout(speakerBar);

    // ---- 音频电平检测：麦克风输入 / 扬声器输出（-60dB ~ 0dB）----
    QHBoxLayout* levelBar = new QHBoxLayout();
    levelBar->setSpacing(8);

    micLevelBar_ = new QProgressBar(center);
    micLevelBar_->setObjectName("levelMeter");
    micLevelBar_->setRange(0, 100);
    micLevelBar_->setValue(0);
    micLevelBar_->setTextVisible(false);
    micLevelBar_->setFixedHeight(10);
    micLevelBar_->setToolTip(QStringLiteral("麦克风采集到的实时音量"));

    micLevelText_ = new QLabel(QStringLiteral("输入 静音"), center);
    micLevelText_->setObjectName("caption");
    micLevelText_->setMinimumWidth(74);

    speakerLevelBar_ = new QProgressBar(center);
    speakerLevelBar_->setObjectName("levelMeter");
    speakerLevelBar_->setRange(0, 100);
    speakerLevelBar_->setValue(0);
    speakerLevelBar_->setTextVisible(false);
    speakerLevelBar_->setFixedHeight(10);
    speakerLevelBar_->setToolTip(QStringLiteral("正在播放的远端音频音量（含本机静音状态）"));

    speakerLevelText_ = new QLabel(QStringLiteral("输出 静音"), center);
    speakerLevelText_->setObjectName("caption");
    speakerLevelText_->setMinimumWidth(74);

    levelBar->addWidget(micLevelBar_, 1);
    levelBar->addWidget(micLevelText_, 0);
    levelBar->addSpacing(4);
    levelBar->addWidget(speakerLevelBar_, 1);
    levelBar->addWidget(speakerLevelText_, 0);
    deviceArea->addLayout(levelBar);

    centerLayout->addLayout(deviceArea);

    QHBoxLayout* mediaBar = new QHBoxLayout();
    mediaBar->setSpacing(8);
    videoButton_ = new QPushButton(QStringLiteral("摄像头"), center);
    micButton_ = new QPushButton(QStringLiteral("麦克风"), center);
    speakerButton_ = new QPushButton(QStringLiteral("扬声器"), center);
    videoButton_->setCheckable(true);
    micButton_->setCheckable(true);
    speakerButton_->setCheckable(true);

    QLabel* mediaHint = new QLabel(QStringLiteral("RTP 96 = 视频 / 97 = 音频"), center);
    mediaHint->setObjectName("caption");

    mediaBar->addWidget(videoButton_);
    mediaBar->addWidget(micButton_);
    mediaBar->addWidget(speakerButton_);
    mediaBar->addStretch();
    mediaBar->addWidget(mediaHint);
    centerLayout->addLayout(mediaBar);

    // ---- 右：成员与聊天 ----
    QWidget* right = new QWidget(central);
    right->setObjectName("panel");
    right->setFixedWidth(340);
    QVBoxLayout* rightLayout = new QVBoxLayout(right);
    rightLayout->setContentsMargins(14, 14, 14, 14);
    rightLayout->setSpacing(10);

    QLabel* membersTitle = new QLabel(QStringLiteral("成员"), right);
    membersTitle->setObjectName("sectionTitle");
    memberList_ = new QListWidget(right);
    memberList_->setMaximumHeight(170);
    rightLayout->addWidget(membersTitle);
    rightLayout->addWidget(memberList_);

    QLabel* chatTitle = new QLabel(QStringLiteral("聊天"), right);
    chatTitle->setObjectName("sectionTitle");
    chatView_ = new QTextBrowser(right);
    chatView_->setOpenLinks(false);
    rightLayout->addWidget(chatTitle);
    rightLayout->addWidget(chatView_, 1);

    QHBoxLayout* chatRow = new QHBoxLayout();
    chatRow->setSpacing(8);
    chatInput_ = new QLineEdit(right);
    chatInput_->setPlaceholderText(QStringLiteral("输入消息，Ctrl+Enter 发送"));
    sendButton_ = new QPushButton(QStringLiteral("发送"), right);
    sendButton_->setProperty("variant", "primary");
    chatRow->addWidget(chatInput_, 1);
    chatRow->addWidget(sendButton_);
    rightLayout->addLayout(chatRow);

    content->addWidget(left);
    content->addWidget(center, 1);
    content->addWidget(right);
    root->addLayout(content, 1);

    setCentralWidget(central);

    // ---------------- 交互接线 ----------------
    connect(connectButton_, &QPushButton::clicked, this, &MainWindow::showLoginDialog);
    connect(disconnectButton_, &QPushButton::clicked, this, &MainWindow::disconnectRequested);
    connect(refreshButton_, &QPushButton::clicked, this, &MainWindow::onRefreshClicked);
    connect(createButton_, &QPushButton::clicked, this, &MainWindow::onCreateClicked);
    connect(joinButton_, &QPushButton::clicked, this, &MainWindow::onJoinClicked);
    connect(leaveButton_, &QPushButton::clicked, this, &MainWindow::onLeaveClicked);
    connect(sendButton_, &QPushButton::clicked, this, &MainWindow::onSendClicked);

    connect(chatInput_, &QLineEdit::returnPressed, this, &MainWindow::onSendClicked);
    connect(roomList_, &QListWidget::itemSelectionChanged, this, &MainWindow::updateActions);
    connect(roomList_, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem*) {
        onJoinClicked();
    });

    connect(videoButton_, &QPushButton::toggled, this, &MainWindow::videoToggleRequested);
    connect(micButton_, &QPushButton::toggled, this, &MainWindow::micToggleRequested);
    connect(speakerButton_, &QPushButton::toggled, this, &MainWindow::speakerMuteToggleRequested);

    connect(cameraCombo_, qOverload<int>(&QComboBox::currentIndexChanged), this,
            [this](int index) {
                if (updatingDeviceCombos_ || index < 0) return;
                emit cameraDeviceSelected(index);
            });
    connect(micCombo_, qOverload<int>(&QComboBox::currentIndexChanged), this,
            [this](int index) {
                if (updatingDeviceCombos_ || index < 0) return;
                emit micDeviceSelected(index);
            });
    connect(speakerCombo_, qOverload<int>(&QComboBox::currentIndexChanged), this,
            [this](int index) {
                if (updatingDeviceCombos_ || index < 0) return;
                emit speakerDeviceSelected(index);
            });

    QShortcut* sendShortcut = new QShortcut(QKeySequence(QStringLiteral("Ctrl+Return")), this);
    connect(sendShortcut, &QShortcut::activated, this, &MainWindow::onSendClicked);
    QShortcut* refreshShortcut = new QShortcut(QKeySequence(Qt::Key_F5), this);
    connect(refreshShortcut, &QShortcut::activated, this, &MainWindow::onRefreshClicked);
}

void MainWindow::buildStatusBar() {
    statusConn_ = new QLabel(QStringLiteral("未连接"), this);
    statusRoom_ = new QLabel(QStringLiteral("房间：-"), this);
    statusMembers_ = new QLabel(QStringLiteral("成员：0"), this);
    statusMedia_ = new QLabel(QStringLiteral("UDP：-"), this);
    statusStats_ = new QLabel(QStringLiteral("本地 0 FPS / 0 kbps · 远端 0 FPS / 0 kbps"), this);

    for (QLabel* label : {statusConn_, statusRoom_, statusMembers_, statusMedia_, statusStats_}) {
        label->setObjectName("statusLabel");
    }

    statusBar()->addPermanentWidget(statusConn_);
    statusBar()->addPermanentWidget(statusRoom_);
    statusBar()->addPermanentWidget(statusMembers_);
    statusBar()->addPermanentWidget(statusMedia_);
    statusBar()->addPermanentWidget(statusStats_);
    statusBar()->showMessage(QStringLiteral("就绪"), 0);
}

void MainWindow::connectModelSignals() {
    connect(model_, &AppModel::connStateChanged, this, &MainWindow::onConnStateChanged);
    connect(model_, &AppModel::roomsChanged, this, &MainWindow::onRoomsChanged);
    connect(model_, &AppModel::currentRoomChanged, this, &MainWindow::onRoomChanged);
    connect(model_, &AppModel::membersChanged, this, &MainWindow::onMembersChanged);
    connect(model_, &AppModel::chatChanged, this, &MainWindow::onChatChanged);
    connect(model_, &AppModel::mediaStateChanged, this, &MainWindow::onMediaStateChanged);
    connect(model_, &AppModel::mediaStatsChanged, this, &MainWindow::onMediaStatsChanged);
    connect(model_, &AppModel::statusTextChanged, this, &MainWindow::onStatusTextChanged);
}

// ============================ 登录与连接 ============================

void MainWindow::showLoginDialog() {
    LoginDialog::Credentials defaults;
    defaults.host = model_->serverHost().isEmpty() ? QStringLiteral("127.0.0.1")
                                                  : model_->serverHost();
    defaults.port = model_->serverPort() == 0 ? 8888 : model_->serverPort();
    defaults.userId = model_->selfUserId();
    defaults.userName = model_->selfUserName();

    LoginDialog dialog(this);
    dialog.setCredentials(defaults);
    if (dialog.exec() != QDialog::Accepted) return;

    const LoginDialog::Credentials credentials = dialog.credentials();
    emit loginRequested(credentials.host, credentials.port,
                        credentials.userId, credentials.userName);
}

void MainWindow::showError(const QString& message) {
    statusBar()->showMessage(message, 6000);
    QMessageBox::warning(this, QStringLiteral("提示"), message);
}

void MainWindow::setLocalFrame(const QImage& image) { localView_->setFrame(image); }

void MainWindow::setRemoteFrame(const QImage& image) { remoteView_->setFrame(image); }

void MainWindow::setMediaDevices(const QStringList& cameras, int cameraIndex,
                                 const QStringList& microphones, int micIndex,
                                 const QStringList& speakers, int speakerIndex) {
    updatingDeviceCombos_ = true;

    cameraCombo_->clear();
    if (cameras.isEmpty()) {
        cameraCombo_->addItem(QStringLiteral("未检测到摄像头"));
        cameraCombo_->setEnabled(false);
    } else {
        cameraCombo_->addItems(cameras);
        cameraCombo_->setCurrentIndex(qBound(0, cameraIndex, cameras.size() - 1));
        cameraCombo_->setEnabled(true);
    }

    micCombo_->clear();
    if (microphones.isEmpty()) {
        micCombo_->addItem(QStringLiteral("未检测到麦克风"));
        micCombo_->setEnabled(false);
    } else {
        micCombo_->addItems(microphones);
        micCombo_->setCurrentIndex(qBound(0, micIndex, microphones.size() - 1));
        micCombo_->setEnabled(true);
    }

    speakerCombo_->clear();
    if (speakers.isEmpty()) {
        speakerCombo_->addItem(QStringLiteral("未检测到扬声器"));
        speakerCombo_->setEnabled(false);
    } else {
        speakerCombo_->addItems(speakers);
        speakerCombo_->setCurrentIndex(qBound(0, speakerIndex, speakers.size() - 1));
        speakerCombo_->setEnabled(true);
    }

    updatingDeviceCombos_ = false;
}

void MainWindow::setAudioLevels(double inputLevel, double outputLevel) {
    // 0.0~1.0 线性映射到 -60dB~0dB；>=0.9 视为接近削波，进度条转红
    auto apply = [](QProgressBar* bar, QLabel* text, double level,
                    const QString& prefix, bool muted) {
        const double shown = muted ? 0.0 : level;
        const int value = qBound(0, static_cast<int>(shown * 100.0 + 0.5), 100);
        bar->setValue(value);

        const bool hot = shown >= 0.9;
        if (bar->property("hot").toBool() != hot) {
            bar->setProperty("hot", hot);
            bar->style()->unpolish(bar);
            bar->style()->polish(bar);
        }

        if (muted || shown <= 0.02) {
            text->setText(prefix + QStringLiteral(" 静音"));
        } else {
            text->setText(QStringLiteral("%1 %2 dB").arg(prefix).arg(qRound(shown * 60.0 - 60.0)));
        }
    };

    apply(micLevelBar_, micLevelText_, inputLevel, QStringLiteral("输入"), false);
    apply(speakerLevelBar_, speakerLevelText_, outputLevel, QStringLiteral("输出"),
          model_->speakerMuted());
}

// ============================ 模型变化 -> 界面 ============================

void MainWindow::onConnStateChanged() {
    const AppModel::ConnState state = model_->connState();
    QString text;
    QString styleState = QStringLiteral("off");

    switch (state) {
        case AppModel::ConnState::Disconnected:
            text = QStringLiteral("未连接");
            break;
        case AppModel::ConnState::Connecting:
            text = QStringLiteral("连接中…");
            styleState = QStringLiteral("on");
            break;
        case AppModel::ConnState::Connected:
            text = QStringLiteral("已连接，登录中…");
            styleState = QStringLiteral("on");
            break;
        case AppModel::ConnState::LoggedIn:
            text = QStringLiteral("已登录：%1").arg(model_->selfUserName());
            styleState = QStringLiteral("on");
            break;
    }

    statusConn_->setText(text);
    statusConn_->setProperty("state", styleState);
    statusConn_->style()->unpolish(statusConn_);
    statusConn_->style()->polish(statusConn_);

    if (state == AppModel::ConnState::Disconnected) {
        localView_->clearFrame();
        remoteView_->clearFrame();
    }

    updateActions();
}

void MainWindow::onRoomsChanged() {
    const QString keepRoomId = selectedRoomId();

    QSignalBlocker blocker(roomList_);
    roomList_->clear();
    for (const RoomItem& room : model_->rooms()) {
        QListWidgetItem* item = new QListWidgetItem(roomList_);
        item->setText(QStringLiteral("%1 · %2 人").arg(room.roomName).arg(room.memberCount));
        item->setToolTip(QStringLiteral("房间 ID：%1\n创建者：%2")
                             .arg(room.roomId, room.ownerId));
        item->setData(Qt::UserRole, room.roomId);
    }

    // 尽量保持选中项
    for (int i = 0; i < roomList_->count(); ++i) {
        if (roomList_->item(i)->data(Qt::UserRole).toString() == keepRoomId) {
            roomList_->setCurrentRow(i);
            break;
        }
    }
    updateActions();
}

void MainWindow::onRoomChanged() {
    if (model_->inRoom()) {
        statusRoom_->setText(QStringLiteral("房间：%1").arg(model_->currentRoomName()));
        statusRoom_->setProperty("state", "on");
    } else {
        statusRoom_->setText(QStringLiteral("房间：-"));
        statusRoom_->setProperty("state", "off");
    }
    statusRoom_->style()->unpolish(statusRoom_);
    statusRoom_->style()->polish(statusRoom_);

    remoteView_->setBadge(model_->inRoom() ? QStringLiteral("等待画面") : QString(),
                          model_->inRoom());
    if (!model_->inRoom()) {
        remoteView_->clearFrame();
        remoteView_->setPlaceholderText(QStringLiteral("等待远端画面"));
    }
    updateActions();
}

void MainWindow::onMembersChanged() {
    memberList_->clear();
    for (const MemberItem& member : model_->members()) {
        QString text = member.userName.isEmpty() ? member.userId : member.userName;
        if (member.userId == model_->selfUserId()) text += QStringLiteral("（我）");
        if (member.userId == model_->currentRoomOwnerId()) text += QStringLiteral(" · 房主");

        QListWidgetItem* item = new QListWidgetItem(memberList_);
        item->setText(text);
        item->setToolTip(QStringLiteral("用户 ID：%1").arg(member.userId));
    }
    statusMembers_->setText(QStringLiteral("成员：%1").arg(model_->members().size()));
}

void MainWindow::onChatChanged() {
    QString html = QStringLiteral(
        "<div style='font-family:\"Microsoft YaHei UI\";font-size:16px;color:#E6EDF3;'>");
    for (const ChatMessage& msg : model_->chatMessages()) {
        const QString time = QDateTime::fromSecsSinceEpoch(msg.timestamp)
                                 .toString(QStringLiteral("HH:mm:ss"));
        const QString nameColor = msg.self ? theme::success().name() : theme::primary().name();
        const QString nameSuffix = msg.self ? QStringLiteral("（我）") : QString();

        html += QStringLiteral(
                    "<p style='margin:0 0 8px 0;'>"
                    "<span style='color:%1;font-weight:600;'>%2%3</span>"
                    "<span style='color:%4;font-size:12px;'> %5</span><br/>"
                    "<span style='color:%6;'>%7</span></p>")
                    .arg(nameColor, msg.userName.toHtmlEscaped(), nameSuffix,
                         theme::textMuted().name(), time,
                         theme::text().name(), msg.content.toHtmlEscaped());
    }
    html += QStringLiteral("</div>");

    chatView_->setHtml(html);
    chatView_->verticalScrollBar()->setValue(chatView_->verticalScrollBar()->maximum());
}

void MainWindow::onMediaStateChanged() {
    QSignalBlocker blockVideo(videoButton_);
    QSignalBlocker blockMic(micButton_);
    QSignalBlocker blockSpeaker(speakerButton_);

    videoButton_->setChecked(model_->videoSendEnabled());
    micButton_->setChecked(model_->micEnabled());
    speakerButton_->setChecked(model_->speakerMuted());

    videoButton_->setText(model_->videoSendEnabled() ? QStringLiteral("摄像头 开")
                                                     : QStringLiteral("摄像头 关"));
    micButton_->setText(model_->micEnabled() ? QStringLiteral("麦克风 开")
                                             : QStringLiteral("麦克风 关"));
    speakerButton_->setText(model_->speakerMuted() ? QStringLiteral("扬声器 静音")
                                                   : QStringLiteral("扬声器 有声"));

    localView_->setBadge(model_->videoSendEnabled() ? QStringLiteral("发送中") : QString(),
                         model_->videoSendEnabled());

    if (!model_->videoSendEnabled()) {
        localView_->setPlaceholderText(QStringLiteral("摄像头未开启"));
        localView_->clearFrame();
    } else {
        localView_->setPlaceholderText(QStringLiteral("正在启动摄像头…"));
    }

    statusMedia_->setText(model_->mediaPort() == 0
                              ? QStringLiteral("UDP：-")
                              : QStringLiteral("UDP：%1").arg(model_->mediaPort()));

    micButton_->setToolTip(model_->hasAudioInput()
                               ? QStringLiteral("采集 48kHz 双声道 PCM，AAC 编码后通过 RTP 发送")
                               : QStringLiteral("本机未检测到音频输入设备"));

    updateActions();
}

void MainWindow::onMediaStatsChanged() {
    statusStats_->setText(
        QStringLiteral("本地 %1 FPS / %2 kbps · 远端 %3 FPS / %4 kbps")
            .arg(model_->localFps(), 0, 'f', 0)
            .arg(model_->localKbps())
            .arg(model_->remoteFps(), 0, 'f', 0)
            .arg(model_->remoteKbps()));
}

void MainWindow::onStatusTextChanged(const QString& text) {
    if (text.isEmpty()) return;
    statusBar()->showMessage(text, 8000);
}

// ============================ 用户操作 -> 控制器信号 ============================

QString MainWindow::selectedRoomId() const {
    QListWidgetItem* item = roomList_->currentItem();
    if (!item) return QString();
    return item->data(Qt::UserRole).toString();
}

void MainWindow::updateActions() {
    const bool loggedIn = model_->isLoggedIn();
    const bool inRoom = model_->inRoom();
    const bool hasSelection = !selectedRoomId().isEmpty();

    refreshButton_->setEnabled(loggedIn);
    createButton_->setEnabled(loggedIn);
    joinButton_->setEnabled(loggedIn && hasSelection);
    leaveButton_->setEnabled(loggedIn && inRoom);
    disconnectButton_->setEnabled(model_->connState() != AppModel::ConnState::Disconnected);

    videoButton_->setEnabled(loggedIn);
    micButton_->setEnabled(loggedIn && model_->hasAudioInput());
    speakerButton_->setEnabled(loggedIn);

    sendButton_->setEnabled(loggedIn && inRoom);
    chatInput_->setEnabled(loggedIn && inRoom);
}

void MainWindow::onRefreshClicked() {
    if (!model_->isLoggedIn()) {
        showError(QStringLiteral("请先登录"));
        return;
    }
    emit refreshRoomsRequested();
}

void MainWindow::onCreateClicked() {
    if (!model_->isLoggedIn()) {
        showError(QStringLiteral("请先登录"));
        return;
    }

    bool ok = false;
    const QString roomId = QInputDialog::getText(this, QStringLiteral("创建房间"),
                                                 QStringLiteral("房间 ID（唯一标识，不能为空）"),
                                                 QLineEdit::Normal, QString(), &ok)
                               .trimmed();
    if (!ok || roomId.isEmpty()) return;

    const QString roomName =
        QInputDialog::getText(this, QStringLiteral("创建房间"),
                              QStringLiteral("房间名称（可留空，默认用房间 ID）"),
                              QLineEdit::Normal, roomId, &ok)
            .trimmed();
    if (!ok) return;

    emit createRoomRequested(roomId, roomName);
}

void MainWindow::onJoinClicked() {
    const QString roomId = selectedRoomId();
    if (roomId.isEmpty()) {
        showError(QStringLiteral("请先在左侧选择一个房间"));
        return;
    }
    emit joinRoomRequested(roomId);
}

void MainWindow::onLeaveClicked() {
    if (!model_->inRoom()) return;

    const QMessageBox::StandardButton answer = QMessageBox::question(
        this, QStringLiteral("离开房间"),
        QStringLiteral("确定离开房间「%1」吗？").arg(model_->currentRoomName()),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (answer != QMessageBox::Yes) return;

    emit leaveRoomRequested();
}

void MainWindow::onSendClicked() {
    const QString text = chatInput_->text().trimmed();
    if (text.isEmpty()) return;
    if (!model_->inRoom()) {
        showError(QStringLiteral("请先加入一个房间"));
        return;
    }
    emit sendChatRequested(text);
    chatInput_->clear();
}
