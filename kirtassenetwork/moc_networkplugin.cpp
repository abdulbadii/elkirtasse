/****************************************************************************
** Meta object code from reading C++ file 'networkplugin.h'
**
** Created: Wed Nov 16 20:36:11 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "networkplugin.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'networkplugin.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_networkplugin[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      23,   14,   15,   14, 0x0a,
      36,   14,   15,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_networkplugin[] = {
    "networkplugin\0\0QString\0execPlugin()\0"
    "pluginTitle()\0"
};

const QMetaObject networkplugin::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_networkplugin,
      qt_meta_data_networkplugin, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &networkplugin::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *networkplugin::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *networkplugin::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_networkplugin))
        return static_cast<void*>(const_cast< networkplugin*>(this));
    if (!strcmp(_clname, "NetInterface"))
        return static_cast< NetInterface*>(const_cast< networkplugin*>(this));
    if (!strcmp(_clname, "com.elirtasse.Plugin.NetInterface/1.0"))
        return static_cast< NetInterface*>(const_cast< networkplugin*>(this));
    return QObject::qt_metacast(_clname);
}

int networkplugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: { QString _r = execPlugin();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 1: { QString _r = pluginTitle();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
