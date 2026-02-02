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
            QString userId = UserBase->getUserId(splitetedUserData[1]);

            sendAuthMessage(userSenderSocket, userId, true);
            authorizedUsers[userSenderSocket] = userId.toInt();
            sendUserFullName(userSenderSocket);

        }
        else
        {
            sendAuthMessage(userSenderSocket, nullptr,false);
        }
    }
    if(userMessage.startsWith("MSG"))
    {
        QStringList splitetedUserMSG = userMessage.split("|");
        qDebug() << "[Server] "  << splitetedUserMSG[1] << splitetedUserMSG[2] << splitetedUserMSG[3] ;
        UserBase->saveMessageToBase(splitetedUserMSG[1].toInt(), splitetedUserMSG[2].toInt(), splitetedUserMSG[3]);
        broadcastPrivateMessage(splitetedUserMSG[1].toInt(), splitetedUserMSG[2].toInt(), splitetedUserMSG[3]);
    }
    if(userMessage.startsWith("CHAT"))
    {
        QStringList usersIdsSplitedData = userMessage.split("|");

        int sender = usersIdsSplitedData[1].toInt();
        int reciver = usersIdsSplitedData[2].toInt();
        int gettedUsersChatId = UserBase->getChatIdBetweenUsers(sender, reciver);
        QByteArray usersMessagesInChat = UserBase->getMessages(gettedUsersChatId);
        sendChatData(userSenderSocket, usersMessagesInChat);
    }
}


void Server::broadcastPrivateMessage(int senderId, int reciverId,QString message )
{
    QString fullMessage = "MSG|" + QString::number(senderId)  + "|" + message;

    for(auto socketIter = authorizedUsers.begin(); socketIter != authorizedUsers.end(); ++socketIter )
    {
        int userId = socketIter.value();
        QTcpSocket* socket = socketIter.key();

        if(userId != reciverId)
            continue;

        socket->write(fullMessage.toUtf8());
        socket->flush();
        break;
    }


}

void Server::sendAuthMessage(QTcpSocket* userAutSocket,QString userId, bool isAauthenticated)
{
    QString autMessage = "AUT|";
    if(isAauthenticated)
    {
        autMessage += "Success|" + userId;
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

void Server::sendChatData(QTcpSocket* userSocketForChatHistory, QByteArray chatData)
{
    QString allMessages = "CHAT|";
    userSocketForChatHistory->write(allMessages.toUtf8() + chatData);
    userSocketForChatHistory->flush();
}

