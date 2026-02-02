#ifndef USERDATABASE_H
#define USERDATABASE_H

#include <QString>
#include <QDebug>

#include <QSqlDataBase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>


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
    QByteArray userNameSernameForSending();
    bool auntificate(const QString& login , const QString& password);
    QString getUserId(const QString& login);
    int createChat();
    bool addMessage(int chatId, int senderId, const QString& text);
    bool addUserToChat(int chatId, int userId);
    int getChatIdBetweenUsers(int user1, int user2);
    bool saveMessageToBase(int senderId, int receiverId, const QString &text);
    QByteArray getMessages(int chatId);

private:
    QSqlDatabase usersBase;
};

#endif // USERDATABASE_H
