#include "mainwindow.h"
#include "loding_wd.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // 设置应用程序图标
    a.setWindowIcon(QIcon(":/new/prefix1/icon/app_icon.svg"));
    
    // 加载QSS样式表
    QFile styleFile(":/new/prefix1/styles.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QTextStream stream(&styleFile);
        QString styleSheet = stream.readAll();
        a.setStyleSheet(styleSheet);
        styleFile.close();
    }
    
    loding_wd Lo;
    //MainWindow wd;
    Lo.show();

    return a.exec();
}
