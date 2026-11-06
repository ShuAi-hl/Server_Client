/****************************************************************************
** Meta object code from reading C++ file 'MediaEngine.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/model/MediaEngine.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MediaEngine.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN11MediaEngineE_t {};
} // unnamed namespace

template <> constexpr inline auto MediaEngine::qt_create_metaobjectdata<qt_meta_tag_ZN11MediaEngineE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "MediaEngine",
        "portBound",
        "",
        "port",
        "localFrame",
        "QImage",
        "image",
        "remoteFrame",
        "statsChanged",
        "localFps",
        "localKbps",
        "remoteFps",
        "remoteKbps",
        "warning",
        "message",
        "videoSendStateChanged",
        "on",
        "audioSendStateChanged",
        "onWorkerPreview",
        "onWorkerVideoEncoded",
        "annexb",
        "onWorkerRemoteImage",
        "onWorkerWarning"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'portBound'
        QtMocHelpers::SignalData<void(quint16)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::UShort, 3 },
        }}),
        // Signal 'localFrame'
        QtMocHelpers::SignalData<void(const QImage &)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 5, 6 },
        }}),
        // Signal 'remoteFrame'
        QtMocHelpers::SignalData<void(const QImage &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 5, 6 },
        }}),
        // Signal 'statsChanged'
        QtMocHelpers::SignalData<void(double, int, double, int)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 9 }, { QMetaType::Int, 10 }, { QMetaType::Double, 11 }, { QMetaType::Int, 12 },
        }}),
        // Signal 'warning'
        QtMocHelpers::SignalData<void(const QString &)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Signal 'videoSendStateChanged'
        QtMocHelpers::SignalData<void(bool)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 16 },
        }}),
        // Signal 'audioSendStateChanged'
        QtMocHelpers::SignalData<void(bool)>(17, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 16 },
        }}),
        // Slot 'onWorkerPreview'
        QtMocHelpers::SlotData<void(const QImage &)>(18, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 5, 6 },
        }}),
        // Slot 'onWorkerVideoEncoded'
        QtMocHelpers::SlotData<void(const QByteArray &)>(19, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QByteArray, 20 },
        }}),
        // Slot 'onWorkerRemoteImage'
        QtMocHelpers::SlotData<void(const QImage &)>(21, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 5, 6 },
        }}),
        // Slot 'onWorkerWarning'
        QtMocHelpers::SlotData<void(const QString &)>(22, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<MediaEngine, qt_meta_tag_ZN11MediaEngineE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject MediaEngine::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11MediaEngineE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11MediaEngineE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN11MediaEngineE_t>.metaTypes,
    nullptr
} };

void MediaEngine::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MediaEngine *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->portBound((*reinterpret_cast<std::add_pointer_t<quint16>>(_a[1]))); break;
        case 1: _t->localFrame((*reinterpret_cast<std::add_pointer_t<QImage>>(_a[1]))); break;
        case 2: _t->remoteFrame((*reinterpret_cast<std::add_pointer_t<QImage>>(_a[1]))); break;
        case 3: _t->statsChanged((*reinterpret_cast<std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[4]))); break;
        case 4: _t->warning((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->videoSendStateChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 6: _t->audioSendStateChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 7: _t->onWorkerPreview((*reinterpret_cast<std::add_pointer_t<QImage>>(_a[1]))); break;
        case 8: _t->onWorkerVideoEncoded((*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[1]))); break;
        case 9: _t->onWorkerRemoteImage((*reinterpret_cast<std::add_pointer_t<QImage>>(_a[1]))); break;
        case 10: _t->onWorkerWarning((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (MediaEngine::*)(quint16 )>(_a, &MediaEngine::portBound, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (MediaEngine::*)(const QImage & )>(_a, &MediaEngine::localFrame, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (MediaEngine::*)(const QImage & )>(_a, &MediaEngine::remoteFrame, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (MediaEngine::*)(double , int , double , int )>(_a, &MediaEngine::statsChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (MediaEngine::*)(const QString & )>(_a, &MediaEngine::warning, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (MediaEngine::*)(bool )>(_a, &MediaEngine::videoSendStateChanged, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (MediaEngine::*)(bool )>(_a, &MediaEngine::audioSendStateChanged, 6))
            return;
    }
}

const QMetaObject *MediaEngine::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MediaEngine::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11MediaEngineE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int MediaEngine::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void MediaEngine::portBound(quint16 _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void MediaEngine::localFrame(const QImage & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void MediaEngine::remoteFrame(const QImage & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void MediaEngine::statsChanged(double _t1, int _t2, double _t3, int _t4)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1, _t2, _t3, _t4);
}

// SIGNAL 4
void MediaEngine::warning(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void MediaEngine::videoSendStateChanged(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void MediaEngine::audioSendStateChanged(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1);
}
QT_WARNING_POP
