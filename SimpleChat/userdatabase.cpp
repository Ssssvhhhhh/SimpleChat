#include "userdatabase.h"

UserDataBase::UserDataBase()
{
    usersBase = QSqlDatabase::addDatabase("QSQLITE", "UserDB");
    usersBase.setDatabaseName("SQLUserDataBase.db");

    if (!usersBase.open())
    {
        qDebug() << "[DataBase] OPEN ERROR:" << usersBase.lastError().text();
    } else {
        qDebug() << "[DataBase] Database opened";
    }
    qDebug() << "DB FILE:" << usersBase.databaseName();
    qDebug() << "Tables:" << usersBase.tables();

}

void UserDataBase::addUserInDataBase(userData data)
{

    QSqlQuery addQuery(usersBase);

    addQuery.prepare(
        "INSERT INTO UserBase (login, userpassword, name, sername, email) "
        "VALUES (?, ?, ?, ?, ?)"
        );

    addQuery.addBindValue(data.login);
    addQuery.addBindValue(data.password);
    addQuery.addBindValue(data.name);
    addQuery.addBindValue(data.sername);
    addQuery.addBindValue(data.email);

    if (!addQuery.exec())
    {
        qDebug() << "[DataBase] user addition error:" << addQuery.lastError().text();
    }
    else
    {
        qDebug() << "[DataBase] user added OK";
    }
}

bool UserDataBase::auntificate(const QString &login, const QString &password)
{
    QSqlQuery authenticateQuery(usersBase);

    authenticateQuery.prepare(
        "SELECT 1 FROM UserBase "
        "WHERE login = :Login AND userpassword = :Userpassword");

    authenticateQuery.bindValue(":Login", login);
    authenticateQuery.bindValue(":Userpassword", password);

    if (!authenticateQuery.exec())
    {
        qDebug() << "[DataBase] user auntificate error:" << authenticateQuery.lastError().text();
    }


    if(authenticateQuery.next())
    {
        qDebug() << "[DataBase] user auntificated";
        return true;
    }
    else
    {
        qDebug() << "[DataBase] user not auntificated";
        return false;
    }

}
