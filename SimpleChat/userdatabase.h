#ifndef USERDATABASE_H
#define USERDATABASE_H

#include <QString>
#include <QDebug>

#include <QSqlDataBase>
#include <QSqlQuery>
#include <QSqlError>



class UserDataBase
{
public:
    UserDataBase();
    struct userData
    {
        QString login;
        QString password;
        QString name;
        QString sername;
        QString email;
    };

    void addUserInDataBase(userData data);
private:
    QSqlDatabase usersBase;
};

#endif // USERDATABASE_H
