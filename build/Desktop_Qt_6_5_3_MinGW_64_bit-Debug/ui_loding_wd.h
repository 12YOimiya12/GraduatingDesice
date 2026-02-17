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
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_loding_wd
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QHBoxLayout *horizontalLayout;
    QLabel *label_2;
    QLineEdit *lineEdit;
    QPushButton *pushButton_confirm;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_3;
    QLineEdit *lineEdit_2;
    QPushButton *pushButton_cancel;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *loding_wd)
    {
        if (loding_wd->objectName().isEmpty())
            loding_wd->setObjectName("loding_wd");
        loding_wd->resize(267, 223);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/new/prefix1/icon/folder_icon-icons.com_55318.png"), QSize(), QIcon::Normal, QIcon::Off);
        loding_wd->setWindowIcon(icon);
        centralwidget = new QWidget(loding_wd);
        centralwidget->setObjectName("centralwidget");
        verticalLayout_2 = new QVBoxLayout(centralwidget);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        label = new QLabel(centralwidget);
        label->setObjectName("label");
        QFont font;
        font.setPointSize(34);
        label->setFont(font);

        verticalLayout->addWidget(label);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName("label_2");

        horizontalLayout->addWidget(label_2);

        lineEdit = new QLineEdit(centralwidget);
        lineEdit->setObjectName("lineEdit");

        horizontalLayout->addWidget(lineEdit);

        pushButton_confirm = new QPushButton(centralwidget);
        pushButton_confirm->setObjectName("pushButton_confirm");

        horizontalLayout->addWidget(pushButton_confirm);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName("label_3");

        horizontalLayout_2->addWidget(label_3);

        lineEdit_2 = new QLineEdit(centralwidget);
        lineEdit_2->setObjectName("lineEdit_2");

        horizontalLayout_2->addWidget(lineEdit_2);

        pushButton_cancel = new QPushButton(centralwidget);
        pushButton_cancel->setObjectName("pushButton_cancel");

        horizontalLayout_2->addWidget(pushButton_cancel);


        verticalLayout->addLayout(horizontalLayout_2);


        verticalLayout_2->addLayout(verticalLayout);

        loding_wd->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(loding_wd);
        statusbar->setObjectName("statusbar");
        loding_wd->setStatusBar(statusbar);

        retranslateUi(loding_wd);

        QMetaObject::connectSlotsByName(loding_wd);
    } // setupUi

    void retranslateUi(QMainWindow *loding_wd)
    {
        loding_wd->setWindowTitle(QCoreApplication::translate("loding_wd", "\347\231\273\345\205\245\347\252\227\345\217\243", nullptr));
        label->setText(QCoreApplication::translate("loding_wd", "\350\257\267\350\276\223\345\205\245\350\264\246\345\217\267\345\257\206\347\240\201", nullptr));
        label_2->setText(QCoreApplication::translate("loding_wd", "\350\264\246\345\217\267\357\274\232", nullptr));
        pushButton_confirm->setText(QCoreApplication::translate("loding_wd", "\347\241\256\350\256\244", nullptr));
        label_3->setText(QCoreApplication::translate("loding_wd", "\345\257\206\347\240\201\357\274\232", nullptr));
        pushButton_cancel->setText(QCoreApplication::translate("loding_wd", "\345\217\226\346\266\210", nullptr));
    } // retranslateUi

};

namespace Ui {
    class loding_wd: public Ui_loding_wd {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LODING_WD_H
