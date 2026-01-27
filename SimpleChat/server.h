#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class Server : public QTcpServer
{
    Q_OBJECT
public:
    Server(QObject *parent, int port);
    void incomingConnection(qintptr socketDescriptor) override;
    void readClientData();
    void meesageToUser(QTcpSocket* userSocketMessage);
    void userDisconected();

    void broadcastMessage(QTcpSocket* sender, const QString& message);
private:
    //QSet<QTcpSocket*> userSockets;
    QMap<QTcpSocket*, QString> userSocketsAndName;


};

#endif // SERVER_H
