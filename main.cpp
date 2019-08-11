#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QStringList args = a.arguments();

    MainWindow w;
    w.show();

    return a.exec();
}
