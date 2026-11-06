/****************************************************************************
** Meta object code from reading C++ file 'SignalingClient.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/model/SignalingClient.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SignalingClient.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN15SignalingClientE_t {};
} // unnamed namespace

template <> constexpr inline auto SignalingClient::qt_create_metaobjectdata<qt_meta_tag_ZN15SignalingClientE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "SignalingClient",
        "connected",
        "",
        "disconnected",
        "connectionFailed",
        "reason",
        "loginResult",
        "success",
        "message",
        "roomCreated",
        "roomId",
        "roomJoined",
        "QJsonObject",
        "roomInfo",
        "roomLeft",
        "roomListReceived",
        "QJsonArray",
        "rooms",
        "roomInfoReceived",
        "chatReceived",
        "userId",
        "userName",
        "content",
        "timestamp",
        "mediaRegistered",
        "peerSdp",
        "kind",
        "payload",
        "peerIceCandidate",
        "pong",
        "serverMessage",
        "type",
        "errorOccurred",
        "onReadyRead",
        "onConnected",
        "onDisconnected",
        "sendPing"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'connected'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'disconnected'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'connectionFailed'
        QtMocHelpers::SignalData<void(const QString &)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 5 },
        }}),
        // Signal 'loginResult'
        QtMocHelpers::SignalData<void(bool, const QString &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 7 }, { QMetaType::QString, 8 },
        }}),
        // Signal 'roomCreated'
        QtMocHelpers::SignalData<void(bool, const QString &, const QString &)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 7 }, { QMetaType::QString, 10 }, { QMetaType::QString, 8 },
        }}),
        // Signal 'roomJoined'
        QtMocHelpers::SignalData<void(bool, const QJsonObject &, const QString &)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 7 }, { 0x80000000 | 12, 13 }, { QMetaType::QString, 8 },
        }}),
        // Signal 'roomLeft'
        QtMocHelpers::SignalData<void(bool, const QString &)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 7 }, { QMetaType::QString, 8 },
        }}),
        // Signal 'roomListReceived'
        QtMocHelpers::SignalData<void(const QJsonArray &)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 16, 17 },
        }}),
        // Signal 'roomInfoReceived'
        QtMocHelpers::SignalData<void(bool, const QJsonObject &, const QString &)>(18, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 7 }, { 0x80000000 | 12, 13 }, { QMetaType::QString, 8 },
        }}),
        // Signal 'chatReceived'
        QtMocHelpers::SignalData<void(const QString &, const QString &, const QString &, const QString &, qint64)>(19, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 20 }, { QMetaType::QString, 21 }, { QMetaType::QString, 10 }, { QMetaType::QString, 22 },
            { QMetaType::LongLong, 23 },
        }}),
        // Signal 'mediaRegistered'
        QtMocHelpers::SignalData<void(bool, const QString &)>(24, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 7 }, { QMetaType::QString, 8 },
        }}),
        // Signal 'peerSdp'
        QtMocHelpers::SignalData<void(const QString &, const QString &, const QString &)>(25, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 26 }, { QMetaType::QString, 20 }, { QMetaType::QString, 27 },
        }}),
        // Signal 'peerIceCandidate'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(28, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 20 }, { QMetaType::QString, 27 },
        }}),
        // Signal 'pong'
        QtMocHelpers::SignalData<void()>(29, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'serverMessage'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(30, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 31 }, { QMetaType::QString, 22 },
        }}),
        // Signal 'errorOccurred'
        QtMocHelpers::SignalData<void(const QString &)>(32, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 8 },
        }}),
        // Slot 'onReadyRead'
        QtMocHelpers::SlotData<void()>(33, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onConnected'
        QtMocHelpers::SlotData<void()>(34, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDisconnected'
        QtMocHelpers::SlotData<void()>(35, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'sendPing'
        QtMocHelpers::SlotData<void()>(36, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<SignalingClient, qt_meta_tag_ZN15SignalingClientE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject SignalingClient::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15SignalingClientE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15SignalingClientE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN15SignalingClientE_t>.metaTypes,
    nullptr
} };

void SignalingClient::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<SignalingClient *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->connected(); break;
        case 1: _t->disconnected(); break;
        case 2: _t->connectionFailed((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->loginResult((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 4: _t->roomCreated((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 5: _t->roomJoined((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 6: _t->roomLeft((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 7: _t->roomListReceived((*reinterpret_cast<std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 8: _t->roomInfoReceived((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 9: _t->chatReceived((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<qint64>>(_a[5]))); break;
        case 10: _t->mediaRegistered((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 11: _t->peerSdp((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 12: _t->peerIceCandidate((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 13: _t->pong(); break;
        case 14: _t->serverMessage((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 15: _t->errorOccurred((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 16: _t->onReadyRead(); break;
        case 17: _t->onConnected(); break;
        case 18: _t->onDisconnected(); break;
        case 19: _t->sendPing(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (SignalingClient::*)()>(_a, &SignalingClient::connected, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (SignalingClient::*)()>(_a, &SignalingClient::disconnected, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (SignalingClient::*)(const QString & )>(_a, &SignalingClient::connectionFailed, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (SignalingClient::*)(bool , const QString & )>(_a, &SignalingClient::loginResult, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (SignalingClient::*)(bool , const QString & , const QString & )>(_a, &SignalingClient::roomCreated, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (SignalingClient::*)(bool , const QJsonObject & , const QString & )>(_a, &SignalingClient::roomJoined, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (SignalingClient::*)(bool , const QString & )>(_a, &SignalingClient::roomLeft, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (SignalingClient::*)(const QJsonArray & )>(_a, &SignalingClient::roomListReceived, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (SignalingClient::*)(bool , const QJsonObject & , const QString & )>(_a, &SignalingClient::roomInfoReceived, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (SignalingClient::*)(const QString & , const QString & , const QString & , const QString & , qint64 )>(_a, &SignalingClient::chatReceived, 9))
            return;
        if (QtMocHelpers::indexOfMethod<void (SignalingClient::*)(bool , const QString & )>(_a, &SignalingClient::mediaRegistered, 10))
            return;
        if (QtMocHelpers::indexOfMethod<void (SignalingClient::*)(const QString & , const QString & , const QString & )>(_a, &SignalingClient::peerSdp, 11))
            return;
        if (QtMocHelpers::indexOfMethod<void (SignalingClient::*)(const QString & , const QString & )>(_a, &SignalingClient::peerIceCandidate, 12))
            return;
        if (QtMocHelpers::indexOfMethod<void (SignalingClient::*)()>(_a, &SignalingClient::pong, 13))
            return;
        if (QtMocHelpers::indexOfMethod<void (SignalingClient::*)(const QString & , const QString & )>(_a, &SignalingClient::serverMessage, 14))
            return;
        if (QtMocHelpers::indexOfMethod<void (SignalingClient::*)(const QString & )>(_a, &SignalingClient::errorOccurred, 15))
            return;
    }
}

const QMetaObject *SignalingClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SignalingClient::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15SignalingClientE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int SignalingClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 20)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 20;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 20)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 20;
    }
    return _id;
}

// SIGNAL 0
void SignalingClient::connected()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void SignalingClient::disconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void SignalingClient::connectionFailed(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void SignalingClient::loginResult(bool _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1, _t2);
}

// SIGNAL 4
void SignalingClient::roomCreated(bool _t1, const QString & _t2, const QString & _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1, _t2, _t3);
}

// SIGNAL 5
void SignalingClient::roomJoined(bool _t1, const QJsonObject & _t2, const QString & _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1, _t2, _t3);
}

// SIGNAL 6
void SignalingClient::roomLeft(bool _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1, _t2);
}

// SIGNAL 7
void SignalingClient::roomListReceived(const QJsonArray & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1);
}

// SIGNAL 8
void SignalingClient::roomInfoReceived(bool _t1, const QJsonObject & _t2, const QString & _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 8, nullptr, _t1, _t2, _t3);
}

// SIGNAL 9
void SignalingClient::chatReceived(const QString & _t1, const QString & _t2, const QString & _t3, const QString & _t4, qint64 _t5)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 9, nullptr, _t1, _t2, _t3, _t4, _t5);
}

// SIGNAL 10
void SignalingClient::mediaRegistered(bool _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 10, nullptr, _t1, _t2);
}

// SIGNAL 11
void SignalingClient::peerSdp(const QString & _t1, const QString & _t2, const QString & _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 11, nullptr, _t1, _t2, _t3);
}

// SIGNAL 12
void SignalingClient::peerIceCandidate(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 12, nullptr, _t1, _t2);
}

// SIGNAL 13
void SignalingClient::pong()
{
    QMetaObject::activate(this, &staticMetaObject, 13, nullptr);
}

// SIGNAL 14
void SignalingClient::serverMessage(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 14, nullptr, _t1, _t2);
}

// SIGNAL 15
void SignalingClient::errorOccurred(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 15, nullptr, _t1);
}
QT_WARNING_POP
