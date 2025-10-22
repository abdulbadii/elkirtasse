/****************************************************************************
** Meta object code from reading C++ file 'dialognet.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.17)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "dialognet.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dialognet.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.17. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Dialognet_t {
    QByteArrayData data[31];
    char stringdata0[423];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Dialognet_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Dialognet_t qt_meta_stringdata_Dialognet = {
    {
QT_MOC_LITERAL(0, 0, 9), // "Dialognet"
QT_MOC_LITERAL(1, 10, 12), // "downloadFile"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 7), // "urlPath"
QT_MOC_LITERAL(4, 32, 8), // "distPath"
QT_MOC_LITERAL(5, 41, 25), // "on_lineEdit_2_textChanged"
QT_MOC_LITERAL(6, 67, 16), // "searchInTreeview"
QT_MOC_LITERAL(7, 84, 12), // "QTreeWidget*"
QT_MOC_LITERAL(8, 97, 4), // "view"
QT_MOC_LITERAL(9, 102, 12), // "searchString"
QT_MOC_LITERAL(10, 115, 5), // "colum"
QT_MOC_LITERAL(11, 121, 19), // "searchTreeForString"
QT_MOC_LITERAL(12, 141, 16), // "QTreeWidgetItem*"
QT_MOC_LITERAL(13, 158, 6), // "parent"
QT_MOC_LITERAL(14, 165, 7), // "itemtop"
QT_MOC_LITERAL(15, 173, 8), // "topindex"
QT_MOC_LITERAL(16, 182, 12), // "showAllItems"
QT_MOC_LITERAL(17, 195, 34), // "on_treeWidget_itemSelectionCh..."
QT_MOC_LITERAL(18, 230, 21), // "on_buttonBox_accepted"
QT_MOC_LITERAL(19, 252, 21), // "on_toolButton_clicked"
QT_MOC_LITERAL(20, 274, 14), // "cancelDownload"
QT_MOC_LITERAL(21, 289, 19), // "httpRequestFinished"
QT_MOC_LITERAL(22, 309, 14), // "QNetworkReply*"
QT_MOC_LITERAL(23, 324, 5), // "reply"
QT_MOC_LITERAL(24, 330, 22), // "updateDataReadProgress"
QT_MOC_LITERAL(25, 353, 9), // "bytesRead"
QT_MOC_LITERAL(26, 363, 10), // "totalBytes"
QT_MOC_LITERAL(27, 374, 14), // "treeSaveGroupe"
QT_MOC_LITERAL(28, 389, 9), // "sslErrors"
QT_MOC_LITERAL(29, 399, 16), // "QList<QSslError>"
QT_MOC_LITERAL(30, 416, 6) // "errors"

    },
    "Dialognet\0downloadFile\0\0urlPath\0"
    "distPath\0on_lineEdit_2_textChanged\0"
    "searchInTreeview\0QTreeWidget*\0view\0"
    "searchString\0colum\0searchTreeForString\0"
    "QTreeWidgetItem*\0parent\0itemtop\0"
    "topindex\0showAllItems\0"
    "on_treeWidget_itemSelectionChanged\0"
    "on_buttonBox_accepted\0on_toolButton_clicked\0"
    "cancelDownload\0httpRequestFinished\0"
    "QNetworkReply*\0reply\0updateDataReadProgress\0"
    "bytesRead\0totalBytes\0treeSaveGroupe\0"
    "sslErrors\0QList<QSslError>\0errors"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Dialognet[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   79,    2, 0x0a /* Public */,
       5,    1,   84,    2, 0x08 /* Private */,
       6,    3,   87,    2, 0x08 /* Private */,
      11,    6,   94,    2, 0x08 /* Private */,
      16,    2,  107,    2, 0x08 /* Private */,
      17,    0,  112,    2, 0x08 /* Private */,
      18,    0,  113,    2, 0x08 /* Private */,
      19,    0,  114,    2, 0x08 /* Private */,
      20,    0,  115,    2, 0x08 /* Private */,
      21,    1,  116,    2, 0x08 /* Private */,
      24,    2,  119,    2, 0x08 /* Private */,
      27,    1,  124,    2, 0x08 /* Private */,
      28,    2,  127,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Bool, QMetaType::QString, QMetaType::QString,    3,    4,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, 0x80000000 | 7, QMetaType::QString, QMetaType::Int,    8,    9,   10,
    QMetaType::Bool, QMetaType::QString, 0x80000000 | 12, QMetaType::Bool, QMetaType::Int, 0x80000000 | 7, QMetaType::Int,    9,   13,   14,   15,    8,   10,
    QMetaType::Bool, 0x80000000 | 12, 0x80000000 | 7,   13,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 22,   23,
    QMetaType::Void, QMetaType::LongLong, QMetaType::LongLong,   25,   26,
    QMetaType::Bool, 0x80000000 | 7,    8,
    QMetaType::Void, 0x80000000 | 22, 0x80000000 | 29,   23,   30,

       0        // eod
};

void Dialognet::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Dialognet *>(_o);
        (void)_t;
        switch (_id) {
        case 0: { bool _r = _t->downloadFile((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 1: _t->on_lineEdit_2_textChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->searchInTreeview((*reinterpret_cast< QTreeWidget*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 3: { bool _r = _t->searchTreeForString((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< QTreeWidgetItem*(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< QTreeWidget*(*)>(_a[5])),(*reinterpret_cast< int(*)>(_a[6])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 4: { bool _r = _t->showAllItems((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QTreeWidget*(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 5: _t->on_treeWidget_itemSelectionChanged(); break;
        case 6: _t->on_buttonBox_accepted(); break;
        case 7: _t->on_toolButton_clicked(); break;
        case 8: _t->cancelDownload(); break;
        case 9: _t->httpRequestFinished((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 10: _t->updateDataReadProgress((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< qint64(*)>(_a[2]))); break;
        case 11: { bool _r = _t->treeSaveGroupe((*reinterpret_cast< QTreeWidget*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 12: _t->sslErrors((*reinterpret_cast< QNetworkReply*(*)>(_a[1])),(*reinterpret_cast< const QList<QSslError>(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QTreeWidget* >(); break;
            }
            break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 4:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QTreeWidget* >(); break;
            }
            break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QTreeWidget* >(); break;
            }
            break;
        case 9:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkReply* >(); break;
            }
            break;
        case 11:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QTreeWidget* >(); break;
            }
            break;
        case 12:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<QSslError> >(); break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkReply* >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Dialognet::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_Dialognet.data,
    qt_meta_data_Dialognet,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Dialognet::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Dialognet::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Dialognet.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int Dialognet::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
