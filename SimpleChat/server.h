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

    void sendChatData(QTcpSocket* userSocketForChatHistory,QByteArray chatData);
    void userDisconected();
    void broadcastPrivateMessage(int senderId, int reciverId, QString message);
    void broadcastGroupMessage(int senderId, int chatId, const QString& text);
    void broadcastNewGroupChat(int chatId, const QString& chatName, const QList<int>& userIds);

private:
    //QSet<QTcpSocket*> userSockets;
    QMap<QTcpSocket*, int> authorizedUsers;
    UserDataBase* UserBase;

};

#endif // SERVER_H
