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
    MainWindow w2;

    //UserDataBase Base;
    //Base.userNameSernameForSending();
    /*
    UserDataBase::userData Data;
    Data.login = "asd";
    Data.email = "asd";
    Data.name = "asd";
    Data.sername = "asd";
    Data.password = "asd";
    Base.addUserInDataBase(Data);
    */

    w.show();
    w2.show();
    return a.exec();
}
