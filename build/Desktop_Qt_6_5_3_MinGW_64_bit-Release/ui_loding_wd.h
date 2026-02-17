/********************************************************************************
** Form generated from reading UI file 'loding_wd.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LODING_WD_H
#define UI_LODING_WD_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_loding_wd
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *loding_wd)
    {
        if (loding_wd->objectName().isEmpty())
            loding_wd->setObjectName("loding_wd");
        loding_wd->resize(800, 600);
        centralwidget = new QWidget(loding_wd);
        centralwidget->setObjectName("centralwidget");
        loding_wd->setCentralWidget(centralwidget);
        menubar = new QMenuBar(loding_wd);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 19));
        loding_wd->setMenuBar(menubar);
        statusbar = new QStatusBar(loding_wd);
        statusbar->setObjectName("statusbar");
        loding_wd->setStatusBar(statusbar);

        retranslateUi(loding_wd);

        QMetaObject::connectSlotsByName(loding_wd);
    } // setupUi

    void retranslateUi(QMainWindow *loding_wd)
    {
        loding_wd->setWindowTitle(QCoreApplication::translate("loding_wd", "\347\231\273\345\205\245\347\252\227\345\217\243", nullptr));
    } // retranslateUi

};

namespace Ui {
    class loding_wd: public Ui_loding_wd {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LODING_WD_H
