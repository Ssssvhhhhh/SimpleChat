#include "server.h"

Server::Server(QObject* parent) : QTcpServer(parent)
{
    qDebug() << "[Sever]" << "started";
    listen(QHostAddress::Any, 1234);
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket* userSocket = new QTcpSocket(this);
    userSockets.insert(userSocket);

    connect(userSocket, &QTcpSocket::readyRead,this, &Server::readClientData);


}

void Server::readClientData()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) return;

    QByteArray data = clientSocket->readAll();
    if (data.isEmpty()) return;
}
