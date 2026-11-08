// main.cpp - 程序入口：装配 视图 / 控制器 / 模型，加载深色主题
#include <QApplication>

#include "AppController.h"
#include "MainWindow.h"
#include "theme.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Reactor 会议客户端"));

    theme::apply(app);

    AppController controller;
    MainWindow window(controller.model());

    // 硬件设备列表由控制器从媒体层读出后注入视图（视图不直接接触媒体层）
    window.setMediaDevices(controller.cameraDeviceNames(), controller.cameraDeviceIndex(),
                           controller.audioInputDeviceNames(),
                           controller.audioInputDeviceIndex(),
                           controller.audioOutputDeviceNames(),
                           controller.audioOutputDeviceIndex());

    // 视图 -> 控制器（用户操作）
    QObject::connect(&window, &MainWindow::loginRequested,
                     &controller, &AppController::connectToServer);
    QObject::connect(&window, &MainWindow::disconnectRequested,
                     &controller, &AppController::disconnectFromServer);
    QObject::connect(&window, &MainWindow::refreshRoomsRequested,
                     &controller, &AppController::refreshRooms);
    QObject::connect(&window, &MainWindow::createRoomRequested,
                     &controller, &AppController::createRoom);
    QObject::connect(&window, &MainWindow::joinRoomRequested,
                     &controller, &AppController::joinRoom);
    QObject::connect(&window, &MainWindow::leaveRoomRequested,
                     &controller, &AppController::leaveRoom);
    QObject::connect(&window, &MainWindow::sendChatRequested,
                     &controller, &AppController::sendChat);
    QObject::connect(&window, &MainWindow::videoToggleRequested,
                     &controller, &AppController::toggleVideo);
    QObject::connect(&window, &MainWindow::micToggleRequested,
                     &controller, &AppController::toggleMic);
    QObject::connect(&window, &MainWindow::speakerMuteToggleRequested,
                     &controller, &AppController::toggleSpeakerMuted);
    QObject::connect(&window, &MainWindow::cameraDeviceSelected,
                     &controller, &AppController::selectCameraDevice);
    QObject::connect(&window, &MainWindow::micDeviceSelected,
                     &controller, &AppController::selectAudioInputDevice);
    QObject::connect(&window, &MainWindow::speakerDeviceSelected,
                     &controller, &AppController::selectAudioOutputDevice);

    // 控制器 -> 视图（模型 / 媒体变化）
    QObject::connect(&controller, &AppController::localFrame,
                     &window, &MainWindow::setLocalFrame);
    QObject::connect(&controller, &AppController::remoteFrame,
                     &window, &MainWindow::setRemoteFrame);
    QObject::connect(&controller, &AppController::errorOccurred,
                     &window, &MainWindow::showError);
    QObject::connect(&controller, &AppController::audioLevelsChanged,
                     &window, &MainWindow::setAudioLevels);

    window.show();
    window.showLoginDialog();

    return app.exec();
}
