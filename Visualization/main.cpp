#include "MainWindow.h"
#include <QApplication>
#include <QMetaType>

int main(int argc, char *argv[])
{
    qRegisterMetaType<QVector<int> >();
    QApplication a(argc, argv);
    MainWindow w;
    w.showMaximized();

    return a.exec();
}