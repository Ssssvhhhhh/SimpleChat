#include "mainwindow.h"
#include "server.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Server server(nullptr, 1234);
    //Server server2(nullptr, 1235);

    MainWindow w;



    w.show();

    return a.exec();
}
