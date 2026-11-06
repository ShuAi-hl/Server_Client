/****************************************************************************
** Meta object code from reading C++ file 'MainWindow.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/view/MainWindow.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainWindow.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN10MainWindowE_t {};
} // unnamed namespace

template <> constexpr inline auto MainWindow::qt_create_metaobjectdata<qt_meta_tag_ZN10MainWindowE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "MainWindow",
        "loginRequested",
        "",
        "host",
        "port",
        "userId",
        "userName",
        "disconnectRequested",
        "refreshRoomsRequested",
        "createRoomRequested",
        "roomId",
        "roomName",
        "joinRoomRequested",
        "leaveRoomRequested",
        "sendChatRequested",
        "text",
        "videoToggleRequested",
        "on",
        "micToggleRequested",
        "speakerMuteToggleRequested",
        "muted",
        "showLoginDialog",
        "setLocalFrame",
        "QImage",
        "image",
        "setRemoteFrame",
        "showError",
        "message",
        "onConnStateChanged",
        "onRoomsChanged",
        "onRoomChanged",
        "onMembersChanged",
        "onChatChanged",
        "onMediaStateChanged",
        "onMediaStatsChanged",
        "onStatusTextChanged",
        "onRefreshClicked",
        "onCreateClicked",
        "onJoinClicked",
        "onLeaveClicked",
        "onSendClicked"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'loginRequested'
        QtMocHelpers::SignalData<void(const QString &, quint16, const QString &, const QString &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::UShort, 4 }, { QMetaType::QString, 5 }, { QMetaType::QString, 6 },
        }}),
        // Signal 'disconnectRequested'
        QtMocHelpers::SignalData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'refreshRoomsRequested'
        QtMocHelpers::SignalData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'createRoomRequested'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 10 }, { QMetaType::QString, 11 },
        }}),
        // Signal 'joinRoomRequested'
        QtMocHelpers::SignalData<void(const QString &)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 10 },
        }}),
        // Signal 'leaveRoomRequested'
        QtMocHelpers::SignalData<void()>(13, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'sendChatRequested'
        QtMocHelpers::SignalData<void(const QString &)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 15 },
        }}),
        // Signal 'videoToggleRequested'
        QtMocHelpers::SignalData<void(bool)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 17 },
        }}),
        // Signal 'micToggleRequested'
        QtMocHelpers::SignalData<void(bool)>(18, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 17 },
        }}),
        // Signal 'speakerMuteToggleRequested'
        QtMocHelpers::SignalData<void(bool)>(19, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 20 },
        }}),
        // Slot 'showLoginDialog'
        QtMocHelpers::SlotData<void()>(21, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'setLocalFrame'
        QtMocHelpers::SlotData<void(const QImage &)>(22, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 23, 24 },
        }}),
        // Slot 'setRemoteFrame'
        QtMocHelpers::SlotData<void(const QImage &)>(25, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 23, 24 },
        }}),
        // Slot 'showError'
        QtMocHelpers::SlotData<void(const QString &)>(26, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 27 },
        }}),
        // Slot 'onConnStateChanged'
        QtMocHelpers::SlotData<void()>(28, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRoomsChanged'
        QtMocHelpers::SlotData<void()>(29, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRoomChanged'
        QtMocHelpers::SlotData<void()>(30, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onMembersChanged'
        QtMocHelpers::SlotData<void()>(31, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onChatChanged'
        QtMocHelpers::SlotData<void()>(32, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onMediaStateChanged'
        QtMocHelpers::SlotData<void()>(33, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onMediaStatsChanged'
        QtMocHelpers::SlotData<void()>(34, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onStatusTextChanged'
        QtMocHelpers::SlotData<void(const QString &)>(35, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 15 },
        }}),
        // Slot 'onRefreshClicked'
        QtMocHelpers::SlotData<void()>(36, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onCreateClicked'
        QtMocHelpers::SlotData<void()>(37, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onJoinClicked'
        QtMocHelpers::SlotData<void()>(38, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onLeaveClicked'
        QtMocHelpers::SlotData<void()>(39, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSendClicked'
        QtMocHelpers::SlotData<void()>(40, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<MainWindow, qt_meta_tag_ZN10MainWindowE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10MainWindowE_t>.metaTypes,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MainWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->loginRequested((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<quint16>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4]))); break;
        case 1: _t->disconnectRequested(); break;
        case 2: _t->refreshRoomsRequested(); break;
        case 3: _t->createRoomRequested((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 4: _t->joinRoomRequested((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->leaveRoomRequested(); break;
        case 6: _t->sendChatRequested((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->videoToggleRequested((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 8: _t->micToggleRequested((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 9: _t->speakerMuteToggleRequested((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 10: _t->showLoginDialog(); break;
        case 11: _t->setLocalFrame((*reinterpret_cast<std::add_pointer_t<QImage>>(_a[1]))); break;
        case 12: _t->setRemoteFrame((*reinterpret_cast<std::add_pointer_t<QImage>>(_a[1]))); break;
        case 13: _t->showError((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 14: _t->onConnStateChanged(); break;
        case 15: _t->onRoomsChanged(); break;
        case 16: _t->onRoomChanged(); break;
        case 17: _t->onMembersChanged(); break;
        case 18: _t->onChatChanged(); break;
        case 19: _t->onMediaStateChanged(); break;
        case 20: _t->onMediaStatsChanged(); break;
        case 21: _t->onStatusTextChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 22: _t->onRefreshClicked(); break;
        case 23: _t->onCreateClicked(); break;
        case 24: _t->onJoinClicked(); break;
        case 25: _t->onLeaveClicked(); break;
        case 26: _t->onSendClicked(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (MainWindow::*)(const QString & , quint16 , const QString & , const QString & )>(_a, &MainWindow::loginRequested, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (MainWindow::*)()>(_a, &MainWindow::disconnectRequested, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (MainWindow::*)()>(_a, &MainWindow::refreshRoomsRequested, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (MainWindow::*)(const QString & , const QString & )>(_a, &MainWindow::createRoomRequested, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (MainWindow::*)(const QString & )>(_a, &MainWindow::joinRoomRequested, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (MainWindow::*)()>(_a, &MainWindow::leaveRoomRequested, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (MainWindow::*)(const QString & )>(_a, &MainWindow::sendChatRequested, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (MainWindow::*)(bool )>(_a, &MainWindow::videoToggleRequested, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (MainWindow::*)(bool )>(_a, &MainWindow::micToggleRequested, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (MainWindow::*)(bool )>(_a, &MainWindow::speakerMuteToggleRequested, 9))
            return;
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.strings))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 27)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 27;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 27)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 27;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::loginRequested(const QString & _t1, quint16 _t2, const QString & _t3, const QString & _t4)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2, _t3, _t4);
}

// SIGNAL 1
void MainWindow::disconnectRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void MainWindow::refreshRoomsRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void MainWindow::createRoomRequested(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1, _t2);
}

// SIGNAL 4
void MainWindow::joinRoomRequested(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void MainWindow::leaveRoomRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void MainWindow::sendChatRequested(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1);
}

// SIGNAL 7
void MainWindow::videoToggleRequested(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1);
}

// SIGNAL 8
void MainWindow::micToggleRequested(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 8, nullptr, _t1);
}

// SIGNAL 9
void MainWindow::speakerMuteToggleRequested(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 9, nullptr, _t1);
}
QT_WARNING_POP
