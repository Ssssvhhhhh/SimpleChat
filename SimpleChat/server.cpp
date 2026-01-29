#include "server.h"

Server::Server(QObject* parent, int port) : QTcpServer(parent)
{
    qDebug() << "[Sever]" << "started";
    listen(QHostAddress::Any, port);

    UserBase = new UserDataBase();
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket* userSocket = new QTcpSocket(this);
    userSocket->setSocketDescriptor(socketDescriptor);
    //userSockets.insert(userSocket);
    connect(userSocket, &QTcpSocket::readyRead,this, &Server::readClientData);
    connect(userSocket, &QTcpSocket::disconnected,this, &Server::userDisconected);

    qDebug() << "[Server] " << userSocket << " connected";
}

void Server::userDisconected()
{
    QTcpSocket *userSocketSender = qobject_cast<QTcpSocket*>(sender());
    //userSockets.remove(userSocketSender);
    userSocketSender->deleteLater();
    qDebug() << "[SERVER]" << "user disconnecting";
}

void Server::broadcastMessage(QTcpSocket *sender, const QString &message)
{
    //QString fullMessage = message;
    /*
    for(auto iter = userSocketsAndName.begin(); iter != userSocketsAndName.end(); iter++)
    {
        //QTcpSocket* userInChat = iter.key();

    }
    */
}

void Server::readClientData()
{
    QTcpSocket *userSenderSocket = qobject_cast<QTcpSocket*>(sender());

    if (!userSenderSocket) return;

    QByteArray data = userSenderSocket->readAll();
    if (data.isEmpty()){};

    QString userMessage = QString::fromUtf8(data);
    qDebug() << "[Server] "  << "message form user "  << userMessage;

    if(userMessage.startsWith("AUT"))
    {
        QStringList splitetedUserData = userMessage.split("|");
        qDebug() << "[Server] "  << splitetedUserData[1] << splitetedUserData[2];

        if(UserBase->auntificate(splitetedUserData[1],splitetedUserData[2] ))
        {
            sendAuthMessage(userSenderSocket, true);
            authorizedUsers[userSenderSocket] = splitetedUserData[1];
            sendUserFullName(userSenderSocket);
        }
        else
        {
            sendAuthMessage(userSenderSocket, false);
        }
    }
}

void Server::meesageToUser(QTcpSocket *userSocketMessage)
{
    QString messageToUser = "Pong";
    userSocketMessage->write(messageToUser.toUtf8());
    userSocketMessage->flush();

}

void Server::sendAuthMessage(QTcpSocket* userAutSocket, bool isAauthenticated)
{
    QString autMessage = "AUT|";
    if(isAauthenticated)
    {
        autMessage+="Success";
        userAutSocket->write(autMessage.toUtf8());
        userAutSocket->flush();
    }
    else
    {
        autMessage+="Error";
        userAutSocket->write(autMessage.toUtf8());
        userAutSocket->flush();

    }
}

void Server::sendUserFullName(QTcpSocket* userFullNameSocket)
{
    QString fullNameIdentifier = "|FullName|";
    userFullNameSocket->write(fullNameIdentifier.toUtf8() + UserBase->userNameSernameForSending());
    userFullNameSocket->flush();
}

