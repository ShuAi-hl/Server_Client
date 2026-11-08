/****************************************************************************
** Meta object code from reading C++ file 'AppController.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/controller/AppController.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AppController.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN13AppControllerE_t {};
} // unnamed namespace

template <> constexpr inline auto AppController::qt_create_metaobjectdata<qt_meta_tag_ZN13AppControllerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "AppController",
        "localFrame",
        "",
        "QImage",
        "image",
        "remoteFrame",
        "errorOccurred",
        "message",
        "audioLevelsChanged",
        "inputLevel",
        "outputLevel",
        "connectToServer",
        "host",
        "port",
        "userId",
        "userName",
        "disconnectFromServer",
        "refreshRooms",
        "createRoom",
        "roomId",
        "roomName",
        "joinRoom",
        "leaveRoom",
        "sendChat",
        "text",
        "toggleVideo",
        "on",
        "toggleMic",
        "toggleSpeakerMuted",
        "muted",
        "selectCameraDevice",
        "index",
        "selectAudioInputDevice",
        "selectAudioOutputDevice",
        "onSignalingConnected",
        "onSignalingDisconnected",
        "onConnectionFailed",
        "reason",
        "onLoginResult",
        "success",
        "onRoomCreated",
        "onRoomJoined",
        "QJsonObject",
        "roomInfo",
        "onRoomLeft",
        "onRoomList",
        "QJsonArray",
        "rooms",
        "onChatReceived",
        "content",
        "timestamp",
        "onMediaRegistered",
        "onServerMessage",
        "type",
        "onSignalingError",
        "onMediaStats",
        "localFps",
        "localKbps",
        "remoteFps",
        "remoteKbps",
        "onMediaWarning",
        "onVideoSendStateChanged",
        "onAudioSendStateChanged"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'localFrame'
        QtMocHelpers::SignalData<void(const QImage &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'remoteFrame'
        QtMocHelpers::SignalData<void(const QImage &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'errorOccurred'
        QtMocHelpers::SignalData<void(const QString &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
        // Signal 'audioLevelsChanged'
        QtMocHelpers::SignalData<void(double, double)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 9 }, { QMetaType::Double, 10 },
        }}),
        // Slot 'connectToServer'
        QtMocHelpers::SlotData<void(const QString &, quint16, const QString &, const QString &)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 12 }, { QMetaType::UShort, 13 }, { QMetaType::QString, 14 }, { QMetaType::QString, 15 },
        }}),
        // Slot 'disconnectFromServer'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'refreshRooms'
        QtMocHelpers::SlotData<void()>(17, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'createRoom'
        QtMocHelpers::SlotData<void(const QString &, const QString &)>(18, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 19 }, { QMetaType::QString, 20 },
        }}),
        // Slot 'joinRoom'
        QtMocHelpers::SlotData<void(const QString &)>(21, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 19 },
        }}),
        // Slot 'leaveRoom'
        QtMocHelpers::SlotData<void()>(22, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'sendChat'
        QtMocHelpers::SlotData<void(const QString &)>(23, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 24 },
        }}),
        // Slot 'toggleVideo'
        QtMocHelpers::SlotData<void(bool)>(25, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 26 },
        }}),
        // Slot 'toggleMic'
        QtMocHelpers::SlotData<void(bool)>(27, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 26 },
        }}),
        // Slot 'toggleSpeakerMuted'
        QtMocHelpers::SlotData<void(bool)>(28, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 29 },
        }}),
        // Slot 'selectCameraDevice'
        QtMocHelpers::SlotData<void(int)>(30, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 31 },
        }}),
        // Slot 'selectAudioInputDevice'
        QtMocHelpers::SlotData<void(int)>(32, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 31 },
        }}),
        // Slot 'selectAudioOutputDevice'
        QtMocHelpers::SlotData<void(int)>(33, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 31 },
        }}),
        // Slot 'onSignalingConnected'
        QtMocHelpers::SlotData<void()>(34, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSignalingDisconnected'
        QtMocHelpers::SlotData<void()>(35, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onConnectionFailed'
        QtMocHelpers::SlotData<void(const QString &)>(36, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 37 },
        }}),
        // Slot 'onLoginResult'
        QtMocHelpers::SlotData<void(bool, const QString &)>(38, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 39 }, { QMetaType::QString, 7 },
        }}),
        // Slot 'onRoomCreated'
        QtMocHelpers::SlotData<void(bool, const QString &, const QString &)>(40, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 39 }, { QMetaType::QString, 19 }, { QMetaType::QString, 7 },
        }}),
        // Slot 'onRoomJoined'
        QtMocHelpers::SlotData<void(bool, const QJsonObject &, const QString &)>(41, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 39 }, { 0x80000000 | 42, 43 }, { QMetaType::QString, 7 },
        }}),
        // Slot 'onRoomLeft'
        QtMocHelpers::SlotData<void(bool, const QString &)>(44, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 39 }, { QMetaType::QString, 7 },
        }}),
        // Slot 'onRoomList'
        QtMocHelpers::SlotData<void(const QJsonArray &)>(45, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 46, 47 },
        }}),
        // Slot 'onChatReceived'
        QtMocHelpers::SlotData<void(const QString &, const QString &, const QString &, const QString &, qint64)>(48, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 14 }, { QMetaType::QString, 15 }, { QMetaType::QString, 19 }, { QMetaType::QString, 49 },
            { QMetaType::LongLong, 50 },
        }}),
        // Slot 'onMediaRegistered'
        QtMocHelpers::SlotData<void(bool, const QString &)>(51, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 39 }, { QMetaType::QString, 7 },
        }}),
        // Slot 'onServerMessage'
        QtMocHelpers::SlotData<void(const QString &, const QString &)>(52, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 53 }, { QMetaType::QString, 49 },
        }}),
        // Slot 'onSignalingError'
        QtMocHelpers::SlotData<void(const QString &)>(54, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
        // Slot 'onMediaStats'
        QtMocHelpers::SlotData<void(double, int, double, int)>(55, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Double, 56 }, { QMetaType::Int, 57 }, { QMetaType::Double, 58 }, { QMetaType::Int, 59 },
        }}),
        // Slot 'onMediaWarning'
        QtMocHelpers::SlotData<void(const QString &)>(60, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
        // Slot 'onVideoSendStateChanged'
        QtMocHelpers::SlotData<void(bool)>(61, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 26 },
        }}),
        // Slot 'onAudioSendStateChanged'
        QtMocHelpers::SlotData<void(bool)>(62, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 26 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<AppController, qt_meta_tag_ZN13AppControllerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject AppController::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13AppControllerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13AppControllerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN13AppControllerE_t>.metaTypes,
    nullptr
} };

void AppController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AppController *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->localFrame((*reinterpret_cast<std::add_pointer_t<QImage>>(_a[1]))); break;
        case 1: _t->remoteFrame((*reinterpret_cast<std::add_pointer_t<QImage>>(_a[1]))); break;
        case 2: _t->errorOccurred((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->audioLevelsChanged((*reinterpret_cast<std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[2]))); break;
        case 4: _t->connectToServer((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<quint16>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4]))); break;
        case 5: _t->disconnectFromServer(); break;
        case 6: _t->refreshRooms(); break;
        case 7: _t->createRoom((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 8: _t->joinRoom((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 9: _t->leaveRoom(); break;
        case 10: _t->sendChat((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 11: _t->toggleVideo((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 12: _t->toggleMic((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 13: _t->toggleSpeakerMuted((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 14: _t->selectCameraDevice((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 15: _t->selectAudioInputDevice((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 16: _t->selectAudioOutputDevice((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 17: _t->onSignalingConnected(); break;
        case 18: _t->onSignalingDisconnected(); break;
        case 19: _t->onConnectionFailed((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 20: _t->onLoginResult((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 21: _t->onRoomCreated((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 22: _t->onRoomJoined((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 23: _t->onRoomLeft((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 24: _t->onRoomList((*reinterpret_cast<std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 25: _t->onChatReceived((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<qint64>>(_a[5]))); break;
        case 26: _t->onMediaRegistered((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 27: _t->onServerMessage((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 28: _t->onSignalingError((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 29: _t->onMediaStats((*reinterpret_cast<std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[4]))); break;
        case 30: _t->onMediaWarning((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 31: _t->onVideoSendStateChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 32: _t->onAudioSendStateChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (AppController::*)(const QImage & )>(_a, &AppController::localFrame, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)(const QImage & )>(_a, &AppController::remoteFrame, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)(const QString & )>(_a, &AppController::errorOccurred, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppController::*)(double , double )>(_a, &AppController::audioLevelsChanged, 3))
            return;
    }
}

const QMetaObject *AppController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AppController::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13AppControllerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int AppController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 33)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 33;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 33)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 33;
    }
    return _id;
}

// SIGNAL 0
void AppController::localFrame(const QImage & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void AppController::remoteFrame(const QImage & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void AppController::errorOccurred(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void AppController::audioLevelsChanged(double _t1, double _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1, _t2);
}
QT_WARNING_POP
