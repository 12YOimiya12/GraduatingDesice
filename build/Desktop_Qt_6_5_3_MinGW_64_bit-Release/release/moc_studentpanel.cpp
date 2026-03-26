/****************************************************************************
** Meta object code from reading C++ file 'studentpanel.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../studentpanel.h"
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
#error "The header file 'studentpanel.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSCameraDialogENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSCameraDialogENDCLASS = QtMocHelpers::stringData(
    "CameraDialog",
    "onCaptureClicked",
    "",
    "onImageCaptured",
    "id",
    "preview"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSCameraDialogENDCLASS_t {
    uint offsetsAndSizes[12];
    char stringdata0[13];
    char stringdata1[17];
    char stringdata2[1];
    char stringdata3[16];
    char stringdata4[3];
    char stringdata5[8];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSCameraDialogENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSCameraDialogENDCLASS_t qt_meta_stringdata_CLASSCameraDialogENDCLASS = {
    {
        QT_MOC_LITERAL(0, 12),  // "CameraDialog"
        QT_MOC_LITERAL(13, 16),  // "onCaptureClicked"
        QT_MOC_LITERAL(30, 0),  // ""
        QT_MOC_LITERAL(31, 15),  // "onImageCaptured"
        QT_MOC_LITERAL(47, 2),  // "id"
        QT_MOC_LITERAL(50, 7)   // "preview"
    },
    "CameraDialog",
    "onCaptureClicked",
    "",
    "onImageCaptured",
    "id",
    "preview"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSCameraDialogENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   26,    2, 0x08,    1 /* Private */,
       3,    2,   27,    2, 0x08,    2 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::QImage,    4,    5,

       0        // eod
};

Q_CONSTINIT const QMetaObject CameraDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_CLASSCameraDialogENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSCameraDialogENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSCameraDialogENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<CameraDialog, std::true_type>,
        // method 'onCaptureClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onImageCaptured'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QImage &, std::false_type>
    >,
    nullptr
} };

void CameraDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CameraDialog *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onCaptureClicked(); break;
        case 1: _t->onImageCaptured((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QImage>>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject *CameraDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CameraDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSCameraDialogENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int CameraDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSStudentPanelENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSStudentPanelENDCLASS = QtMocHelpers::stringData(
    "StudentPanel",
    "logout",
    "",
    "onLogoutClicked",
    "onRechargeClicked",
    "onRefreshClicked",
    "onTabChanged",
    "index",
    "onWebQueryClicked",
    "onFaceRegisterClicked",
    "onRepairRequestClicked",
    "onRoomChangeClicked",
    "onApplianceControlClicked"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSStudentPanelENDCLASS_t {
    uint offsetsAndSizes[26];
    char stringdata0[13];
    char stringdata1[7];
    char stringdata2[1];
    char stringdata3[16];
    char stringdata4[18];
    char stringdata5[17];
    char stringdata6[13];
    char stringdata7[6];
    char stringdata8[18];
    char stringdata9[22];
    char stringdata10[23];
    char stringdata11[20];
    char stringdata12[26];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSStudentPanelENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSStudentPanelENDCLASS_t qt_meta_stringdata_CLASSStudentPanelENDCLASS = {
    {
        QT_MOC_LITERAL(0, 12),  // "StudentPanel"
        QT_MOC_LITERAL(13, 6),  // "logout"
        QT_MOC_LITERAL(20, 0),  // ""
        QT_MOC_LITERAL(21, 15),  // "onLogoutClicked"
        QT_MOC_LITERAL(37, 17),  // "onRechargeClicked"
        QT_MOC_LITERAL(55, 16),  // "onRefreshClicked"
        QT_MOC_LITERAL(72, 12),  // "onTabChanged"
        QT_MOC_LITERAL(85, 5),  // "index"
        QT_MOC_LITERAL(91, 17),  // "onWebQueryClicked"
        QT_MOC_LITERAL(109, 21),  // "onFaceRegisterClicked"
        QT_MOC_LITERAL(131, 22),  // "onRepairRequestClicked"
        QT_MOC_LITERAL(154, 19),  // "onRoomChangeClicked"
        QT_MOC_LITERAL(174, 25)   // "onApplianceControlClicked"
    },
    "StudentPanel",
    "logout",
    "",
    "onLogoutClicked",
    "onRechargeClicked",
    "onRefreshClicked",
    "onTabChanged",
    "index",
    "onWebQueryClicked",
    "onFaceRegisterClicked",
    "onRepairRequestClicked",
    "onRoomChangeClicked",
    "onApplianceControlClicked"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSStudentPanelENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   74,    2, 0x06,    1 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       3,    0,   75,    2, 0x08,    2 /* Private */,
       4,    0,   76,    2, 0x08,    3 /* Private */,
       5,    0,   77,    2, 0x08,    4 /* Private */,
       6,    1,   78,    2, 0x08,    5 /* Private */,
       8,    0,   81,    2, 0x08,    7 /* Private */,
       9,    0,   82,    2, 0x08,    8 /* Private */,
      10,    0,   83,    2, 0x08,    9 /* Private */,
      11,    0,   84,    2, 0x08,   10 /* Private */,
      12,    0,   85,    2, 0x08,   11 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject StudentPanel::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_CLASSStudentPanelENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSStudentPanelENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSStudentPanelENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<StudentPanel, std::true_type>,
        // method 'logout'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onLogoutClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onRechargeClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onRefreshClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onTabChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onWebQueryClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onFaceRegisterClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onRepairRequestClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onRoomChangeClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onApplianceControlClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void StudentPanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<StudentPanel *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->logout(); break;
        case 1: _t->onLogoutClicked(); break;
        case 2: _t->onRechargeClicked(); break;
        case 3: _t->onRefreshClicked(); break;
        case 4: _t->onTabChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->onWebQueryClicked(); break;
        case 6: _t->onFaceRegisterClicked(); break;
        case 7: _t->onRepairRequestClicked(); break;
        case 8: _t->onRoomChangeClicked(); break;
        case 9: _t->onApplianceControlClicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (StudentPanel::*)();
            if (_t _q_method = &StudentPanel::logout; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *StudentPanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *StudentPanel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSStudentPanelENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int StudentPanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void StudentPanel::logout()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
