#ifndef SERVER_H
#define SERVER_H

#include "userdatabase.h"


#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QStringList>
#include <QSslSocket>
#include <QSslServer>
#include <QSslKey>
#include <QFile>

class Server : public QTcpServer
{
    Q_OBJECT
public:
    Server(QObject *parent, int port);
    void incomingConnection(qintptr socketDescriptor) override;
    void readClientData();
    void sendAuthMessage(QSslSocket* userAutSocket, QString userId, bool isAauthenticated);
    void sendUserFullName(QSslSocket* userFullNameSocket); // поменять название сокета
    void sendUserStatus(int userId);
    void sendChatData(QSslSocket* userSocketForChatHistory,QByteArray chatData);

    void broadcastPrivateMessage(int senderId, int reciverId, QString message);
    void broadcastGroupMessage(int senderId, int chatId, const QString& text);
    void broadcastNewGroupChat(int chatId, const QString& chatName, const QList<int>& userIds);

private slots:
    void userDisconected();

private:
    QSet<QSslSocket*> userSockets;
    QMap<QSslSocket*, int> authorizedUsers;
    QMap<int, QString> onlineUsersIds;
    UserDataBase* UserBase;

    QSslCertificate server_cert;
    QSslKey server_key;

};

#endif // SERVER_H
