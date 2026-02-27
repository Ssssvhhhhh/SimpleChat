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
    userSockets.insert(userSocket);
    connect(userSocket, &QTcpSocket::readyRead,this, &Server::readClientData);
    connect(userSocket, &QTcpSocket::disconnected,this, &Server::userDisconected);


    qDebug() << "[Server] " << userSocket << " connected";
}

void Server::userDisconected()
{
    QTcpSocket *userSocketSender = qobject_cast<QTcpSocket*>(sender());
    userSockets.remove(userSocketSender);
    userSocketSender->deleteLater();
    onlineUsersIds[authorizedUsers[userSocketSender]] = "offline";

    sendUserStatus(authorizedUsers[userSocketSender]);

    authorizedUsers.remove(userSocketSender);
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
            onlineUsersIds[userId.toInt()] = "online";
            sendUserFullName(userSenderSocket);
            sendUserStatus(userId.toInt());
        }
        else
        {
            sendAuthMessage(userSenderSocket, nullptr,false);
        }
    }
    if(userMessage.startsWith("MSG"))
    {
        QStringList splitetedUserMSG = userMessage.split("|");
        qDebug() << "[Server] "  << splitetedUserMSG[2] << splitetedUserMSG[3] << splitetedUserMSG[4] ;
        if(splitetedUserMSG[1] == "private")
        {
            UserBase->saveMessageToBase(splitetedUserMSG[2].toInt(), splitetedUserMSG[3].toInt(), splitetedUserMSG[4]);
            broadcastPrivateMessage(splitetedUserMSG[2].toInt(), splitetedUserMSG[3].toInt(), splitetedUserMSG[4]);
        }
        else
        {
            broadcastGroupMessage(splitetedUserMSG[2].toInt(), splitetedUserMSG[3].toInt(), splitetedUserMSG[4]);
            UserBase->saveGroupMessage(splitetedUserMSG[2].toInt(), splitetedUserMSG[3].toInt(), splitetedUserMSG[4]);
            qDebug() << splitetedUserMSG[3] << " " << splitetedUserMSG[2] << " " << splitetedUserMSG[4];
        }
    }
    if(userMessage.startsWith("CHAT"))
    {
        QStringList usersIdsSplitedData = userMessage.split("|");

        int sender = usersIdsSplitedData[1].toInt();
        int reciver = usersIdsSplitedData[2].toInt();
        int gettedUsersChatId = UserBase->getPrivateChatIdBetweenUsers(sender, reciver);
        qDebug() << "gettedUsersChatId " << gettedUsersChatId;
        QByteArray usersMessagesInChat = UserBase->getMessages(gettedUsersChatId);
        sendChatData(userSenderSocket, usersMessagesInChat);
    }
    if(userMessage.startsWith("GCHAT"))
    {
        QStringList chatIdSplitedData = userMessage.split("|");
        QString messages = "GCHAT|";
        userSenderSocket->write(messages.toUtf8() + UserBase->getGroupMessages(chatIdSplitedData[1].toInt()));
        userSenderSocket->flush();
    }
    if(userMessage.startsWith("CREATE"))
    {
        QList<int> userGroupIds;
        QStringList groupData = userMessage.split("|");
        QString groupName = groupData[1];
        QStringList usersIdsForGroupData = groupData[2].split("&", Qt::SkipEmptyParts);
        for(const QString& idStr : usersIdsForGroupData)
        {
            userGroupIds.append(idStr.toInt());
        }

        qDebug() << "[Server]" << usersIdsForGroupData;
        broadcastNewGroupChat(UserBase->createGroupChat(userGroupIds, groupName), groupName, userGroupIds);
    }
    if(userMessage.startsWith("REG"))
    {
        QStringList registrationSplitedData = userMessage.split("|");
        UserDataBase::userData data;
        data.name = registrationSplitedData[1];
        data.sername = registrationSplitedData[2];
        data.login = registrationSplitedData[3];
        data.password = registrationSplitedData[4];
        data.email = registrationSplitedData[5];

        UserBase->addUserInDataBase(data);
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

void Server::broadcastGroupMessage(int senderId, int chatId, const QString &text)
{
    QList<int> chatUsers = UserBase->getChatUsers(chatId);
    qDebug() << "[SERVER]" << " chat users" << chatUsers;

    for(auto socketIter = authorizedUsers.begin(); socketIter != authorizedUsers.end(); ++socketIter )
    {
        int userId = socketIter.value();
        QTcpSocket* socket = socketIter.key();

        if(!chatUsers.contains(userId) || senderId == userId)
            continue;


        QString message = "GMSG|" + QString::number(senderId) + "|"+ QString::number(chatId)+ "|"+ text;
        socket->write(message.toUtf8());
        socket->flush();
    }
}

void Server::broadcastNewGroupChat(int chatId, const QString &chatName, const QList<int> &userIds)
{
    QString message = "CREATE|" + chatName + "|" + QString::number(chatId);

    for(auto socketIter = authorizedUsers.begin(); socketIter != authorizedUsers.end(); ++socketIter )
    {
        int userId = socketIter.value();
        QTcpSocket* socket = socketIter.key();

        if(!userIds.contains(userId))
            continue;

        socket->write(message.toUtf8());
        socket->flush();
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
    userFullNameSocket->write(fullNameIdentifier.toUtf8() + UserBase->userDataForSending(authorizedUsers[userFullNameSocket], onlineUsersIds) ); // продолжить чтобы отправлялись еще и чаты в которых есть пользователь! на данный момент отправляются тольок имя пользовтелей на серевере но не чаты
    userFullNameSocket->flush();
}

void Server::sendUserStatus(int userId)
{
    QString userSatatus = "STAT|" + QString::number(userId) +"|"+onlineUsersIds[userId];
    for(auto socketIter = authorizedUsers.begin(); socketIter != authorizedUsers.end(); ++socketIter)
    {
        QTcpSocket* userSocket = socketIter.key();
        int id = socketIter.value();
        if(id == userId)
            continue;

        userSocket->write(userSatatus.toUtf8());
        userSocket->flush();
    }
}



void Server::sendChatData(QTcpSocket* userSocketForChatHistory, QByteArray chatData)
{
    QString allMessages = "CHAT|";
    userSocketForChatHistory->write(allMessages.toUtf8() + chatData);
    userSocketForChatHistory->flush();
}

