#include "mainwindow.h"
#include "loding_wd.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    loding_wd Lo;
    //MainWindow wd;
    Lo.show();


    return a.exec();
}
