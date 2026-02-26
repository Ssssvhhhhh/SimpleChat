#include "mainwindow.h"
#include "server.h"
#include "userdatabase.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Server server(nullptr, 1234);
    //Server server2(nullptr, 1235);

    MainWindow w;

    //MainWindow w2;
    //MainWindow w3;

    w.show();
    //w2.show();
    //w3.show();
    return a.exec();
}
