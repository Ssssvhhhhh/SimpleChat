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
    QByteArray userDataForSending(int userId);
    bool auntificate(const QString& login , const QString& password);
    QString getUserId(const QString& login);
    int createChat();
    bool addMessage(int chatId, int senderId, const QString& text);
    bool addUserToChat(int chatId, int userId);
    int getPrivateChatIdBetweenUsers(int user1, int user2);
    bool saveMessageToBase(int senderId, int receiverId, const QString& text);
    bool saveGroupMessage(int senderId, int chatId, const QString& text);
    QByteArray getMessages(int chatId);
    int createGroupChat(const QList<int>& userIds, const QString& groupName);
    QList<int> getChatUsers(int chatId);
    QByteArray getGroupMessages(int chatId);

private:
    QSqlDatabase usersBase;
};

#endif // USERDATABASE_H
