#ifndef SERVER_H
#define SERVER_H

#include "userdatabase.h"


#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QStringList>

class Server : public QTcpServer
{
    Q_OBJECT
public:
    Server(QObject *parent, int port);
    void incomingConnection(qintptr socketDescriptor) override;
    void readClientData();
    void sendAuthMessage(QTcpSocket* userAutSocket, QString userId, bool isAauthenticated);
    void sendUserFullName(QTcpSocket* userFullNameSocket); // поменять название сокета
    void sendUserStatus(int userId);
    void sendChatData(QTcpSocket* userSocketForChatHistory,QByteArray chatData);

    void broadcastPrivateMessage(int senderId, int reciverId, QString message);
    void broadcastGroupMessage(int senderId, int chatId, const QString& text);
    void broadcastNewGroupChat(int chatId, const QString& chatName, const QList<int>& userIds);

private slots:
    void userDisconected();

private:
    QSet<QTcpSocket*> userSockets;
    QMap<QTcpSocket*, int> authorizedUsers;
    QMap<int, QString> onlineUsersIds;
    UserDataBase* UserBase;

};

#endif // SERVER_H
