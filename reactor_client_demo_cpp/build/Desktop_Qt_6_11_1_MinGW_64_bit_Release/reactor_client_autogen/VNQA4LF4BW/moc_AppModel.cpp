/****************************************************************************
** Meta object code from reading C++ file 'AppModel.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/model/AppModel.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AppModel.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN8AppModelE_t {};
} // unnamed namespace

template <> constexpr inline auto AppModel::qt_create_metaobjectdata<qt_meta_tag_ZN8AppModelE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "AppModel",
        "connStateChanged",
        "",
        "AppModel::ConnState",
        "state",
        "serverInfoChanged",
        "selfUserChanged",
        "roomsChanged",
        "currentRoomChanged",
        "membersChanged",
        "chatChanged",
        "mediaStateChanged",
        "mediaStatsChanged",
        "statusTextChanged",
        "text"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'connStateChanged'
        QtMocHelpers::SignalData<void(AppModel::ConnState)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'serverInfoChanged'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'selfUserChanged'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'roomsChanged'
        QtMocHelpers::SignalData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'currentRoomChanged'
        QtMocHelpers::SignalData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'membersChanged'
        QtMocHelpers::SignalData<void()>(9, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'chatChanged'
        QtMocHelpers::SignalData<void()>(10, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'mediaStateChanged'
        QtMocHelpers::SignalData<void()>(11, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'mediaStatsChanged'
        QtMocHelpers::SignalData<void()>(12, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'statusTextChanged'
        QtMocHelpers::SignalData<void(const QString &)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<AppModel, qt_meta_tag_ZN8AppModelE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject AppModel::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8AppModelE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8AppModelE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN8AppModelE_t>.metaTypes,
    nullptr
} };

void AppModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AppModel *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->connStateChanged((*reinterpret_cast<std::add_pointer_t<AppModel::ConnState>>(_a[1]))); break;
        case 1: _t->serverInfoChanged(); break;
        case 2: _t->selfUserChanged(); break;
        case 3: _t->roomsChanged(); break;
        case 4: _t->currentRoomChanged(); break;
        case 5: _t->membersChanged(); break;
        case 6: _t->chatChanged(); break;
        case 7: _t->mediaStateChanged(); break;
        case 8: _t->mediaStatsChanged(); break;
        case 9: _t->statusTextChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (AppModel::*)(AppModel::ConnState )>(_a, &AppModel::connStateChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppModel::*)()>(_a, &AppModel::serverInfoChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppModel::*)()>(_a, &AppModel::selfUserChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppModel::*)()>(_a, &AppModel::roomsChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppModel::*)()>(_a, &AppModel::currentRoomChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppModel::*)()>(_a, &AppModel::membersChanged, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppModel::*)()>(_a, &AppModel::chatChanged, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppModel::*)()>(_a, &AppModel::mediaStateChanged, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppModel::*)()>(_a, &AppModel::mediaStatsChanged, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (AppModel::*)(const QString & )>(_a, &AppModel::statusTextChanged, 9))
            return;
    }
}

const QMetaObject *AppModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AppModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8AppModelE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int AppModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void AppModel::connStateChanged(AppModel::ConnState _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void AppModel::serverInfoChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void AppModel::selfUserChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void AppModel::roomsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void AppModel::currentRoomChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void AppModel::membersChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void AppModel::chatChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void AppModel::mediaStateChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void AppModel::mediaStatsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void AppModel::statusTextChanged(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 9, nullptr, _t1);
}
QT_WARNING_POP
