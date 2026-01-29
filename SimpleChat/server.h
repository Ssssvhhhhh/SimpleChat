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
    void meesageToUser(QTcpSocket* userSocketMessage);
    void sendAuthMessage(QTcpSocket* userAutSocket, bool isAauthenticated);
    void sendUserFullName(QTcpSocket* userFullNameSocket); // поменять название сокета
    void userDisconected();
    void broadcastMessage(QTcpSocket* sender, const QString& message);

private:
    //QSet<QTcpSocket*> userSockets;
    QMap<QTcpSocket*, QString> authorizedUsers;
    UserDataBase* UserBase;

};

#endif // SERVER_H
