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
        // Slot 'connectToServer'
        QtMocHelpers::SlotData<void(const QString &, quint16, const QString &, const QString &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 9 }, { QMetaType::UShort, 10 }, { QMetaType::QString, 11 }, { QMetaType::QString, 12 },
        }}),
        // Slot 'disconnectFromServer'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'refreshRooms'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'createRoom'
        QtMocHelpers::SlotData<void(const QString &, const QString &)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 16 }, { QMetaType::QString, 17 },
        }}),
        // Slot 'joinRoom'
        QtMocHelpers::SlotData<void(const QString &)>(18, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 16 },
        }}),
        // Slot 'leaveRoom'
        QtMocHelpers::SlotData<void()>(19, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'sendChat'
        QtMocHelpers::SlotData<void(const QString &)>(20, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 21 },
        }}),
        // Slot 'toggleVideo'
        QtMocHelpers::SlotData<void(bool)>(22, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 23 },
        }}),
        // Slot 'toggleMic'
        QtMocHelpers::SlotData<void(bool)>(24, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 23 },
        }}),
        // Slot 'toggleSpeakerMuted'
        QtMocHelpers::SlotData<void(bool)>(25, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 26 },
        }}),
        // Slot 'selectCameraDevice'
        QtMocHelpers::SlotData<void(int)>(27, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 28 },
        }}),
        // Slot 'selectAudioInputDevice'
        QtMocHelpers::SlotData<void(int)>(29, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 28 },
        }}),
        // Slot 'onSignalingConnected'
        QtMocHelpers::SlotData<void()>(30, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSignalingDisconnected'
        QtMocHelpers::SlotData<void()>(31, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onConnectionFailed'
        QtMocHelpers::SlotData<void(const QString &)>(32, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 33 },
        }}),
        // Slot 'onLoginResult'
        QtMocHelpers::SlotData<void(bool, const QString &)>(34, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 35 }, { QMetaType::QString, 7 },
        }}),
        // Slot 'onRoomCreated'
        QtMocHelpers::SlotData<void(bool, const QString &, const QString &)>(36, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 35 }, { QMetaType::QString, 16 }, { QMetaType::QString, 7 },
        }}),
        // Slot 'onRoomJoined'
        QtMocHelpers::SlotData<void(bool, const QJsonObject &, const QString &)>(37, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 35 }, { 0x80000000 | 38, 39 }, { QMetaType::QString, 7 },
        }}),
        // Slot 'onRoomLeft'
        QtMocHelpers::SlotData<void(bool, const QString &)>(40, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 35 }, { QMetaType::QString, 7 },
        }}),
        // Slot 'onRoomList'
        QtMocHelpers::SlotData<void(const QJsonArray &)>(41, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 42, 43 },
        }}),
        // Slot 'onChatReceived'
        QtMocHelpers::SlotData<void(const QString &, const QString &, const QString &, const QString &, qint64)>(44, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 11 }, { QMetaType::QString, 12 }, { QMetaType::QString, 16 }, { QMetaType::QString, 45 },
            { QMetaType::LongLong, 46 },
        }}),
        // Slot 'onMediaRegistered'
        QtMocHelpers::SlotData<void(bool, const QString &)>(47, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 35 }, { QMetaType::QString, 7 },
        }}),
        // Slot 'onServerMessage'
        QtMocHelpers::SlotData<void(const QString &, const QString &)>(48, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 49 }, { QMetaType::QString, 45 },
        }}),
        // Slot 'onSignalingError'
        QtMocHelpers::SlotData<void(const QString &)>(50, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
        // Slot 'onMediaStats'
        QtMocHelpers::SlotData<void(double, int, double, int)>(51, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Double, 52 }, { QMetaType::Int, 53 }, { QMetaType::Double, 54 }, { QMetaType::Int, 55 },
        }}),
        // Slot 'onMediaWarning'
        QtMocHelpers::SlotData<void(const QString &)>(56, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
        // Slot 'onVideoSendStateChanged'
        QtMocHelpers::SlotData<void(bool)>(57, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 23 },
        }}),
        // Slot 'onAudioSendStateChanged'
        QtMocHelpers::SlotData<void(bool)>(58, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 23 },
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
        case 3: _t->connectToServer((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<quint16>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4]))); break;
        case 4: _t->disconnectFromServer(); break;
        case 5: _t->refreshRooms(); break;
        case 6: _t->createRoom((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 7: _t->joinRoom((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 8: _t->leaveRoom(); break;
        case 9: _t->sendChat((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 10: _t->toggleVideo((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 11: _t->toggleMic((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 12: _t->toggleSpeakerMuted((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 13: _t->selectCameraDevice((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 14: _t->selectAudioInputDevice((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 15: _t->onSignalingConnected(); break;
        case 16: _t->onSignalingDisconnected(); break;
        case 17: _t->onConnectionFailed((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 18: _t->onLoginResult((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 19: _t->onRoomCreated((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 20: _t->onRoomJoined((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 21: _t->onRoomLeft((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 22: _t->onRoomList((*reinterpret_cast<std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 23: _t->onChatReceived((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<qint64>>(_a[5]))); break;
        case 24: _t->onMediaRegistered((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 25: _t->onServerMessage((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 26: _t->onSignalingError((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 27: _t->onMediaStats((*reinterpret_cast<std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[4]))); break;
        case 28: _t->onMediaWarning((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 29: _t->onVideoSendStateChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 30: _t->onAudioSendStateChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
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
        if (_id < 31)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 31;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 31)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 31;
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
QT_WARNING_POP
