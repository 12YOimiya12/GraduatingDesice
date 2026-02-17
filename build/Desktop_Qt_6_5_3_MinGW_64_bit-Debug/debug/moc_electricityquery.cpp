/****************************************************************************
** Meta object code from reading C++ file 'electricityquery.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../electricityquery.h"
#include <QtGui/qtextcursor.h>
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'electricityquery.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.5.3. It"
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

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSElectricityQueryENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSElectricityQueryENDCLASS = QtMocHelpers::stringData(
    "ElectricityQuery",
    "onOpenBrowserClicked",
    "",
    "onFetchDataClicked",
    "onDataReady",
    "onErrorOccurred",
    "error",
    "onBackClicked"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSElectricityQueryENDCLASS_t {
    uint offsetsAndSizes[16];
    char stringdata0[17];
    char stringdata1[21];
    char stringdata2[1];
    char stringdata3[19];
    char stringdata4[12];
    char stringdata5[16];
    char stringdata6[6];
    char stringdata7[14];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSElectricityQueryENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSElectricityQueryENDCLASS_t qt_meta_stringdata_CLASSElectricityQueryENDCLASS = {
    {
        QT_MOC_LITERAL(0, 16),  // "ElectricityQuery"
        QT_MOC_LITERAL(17, 20),  // "onOpenBrowserClicked"
        QT_MOC_LITERAL(38, 0),  // ""
        QT_MOC_LITERAL(39, 18),  // "onFetchDataClicked"
        QT_MOC_LITERAL(58, 11),  // "onDataReady"
        QT_MOC_LITERAL(70, 15),  // "onErrorOccurred"
        QT_MOC_LITERAL(86, 5),  // "error"
        QT_MOC_LITERAL(92, 13)   // "onBackClicked"
    },
    "ElectricityQuery",
    "onOpenBrowserClicked",
    "",
    "onFetchDataClicked",
    "onDataReady",
    "onErrorOccurred",
    "error",
    "onBackClicked"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSElectricityQueryENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   44,    2, 0x08,    1 /* Private */,
       3,    0,   45,    2, 0x08,    2 /* Private */,
       4,    0,   46,    2, 0x08,    3 /* Private */,
       5,    1,   47,    2, 0x08,    4 /* Private */,
       7,    0,   50,    2, 0x08,    6 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject ElectricityQuery::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_CLASSElectricityQueryENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSElectricityQueryENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSElectricityQueryENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ElectricityQuery, std::true_type>,
        // method 'onOpenBrowserClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onFetchDataClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onDataReady'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onErrorOccurred'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onBackClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void ElectricityQuery::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ElectricityQuery *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onOpenBrowserClicked(); break;
        case 1: _t->onFetchDataClicked(); break;
        case 2: _t->onDataReady(); break;
        case 3: _t->onErrorOccurred((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->onBackClicked(); break;
        default: ;
        }
    }
}

const QMetaObject *ElectricityQuery::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ElectricityQuery::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSElectricityQueryENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int ElectricityQuery::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 5;
    }
    return _id;
}
QT_WARNING_POP
