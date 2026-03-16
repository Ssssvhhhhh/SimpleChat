 #include "server.h"

Server::Server(QObject* parent, int port) : QTcpServer(parent)
{
    qDebug() << "[Sever]" << "started";
    QFile certFile("server.crt");
    QFile keyFile("server.key");

    if (!certFile.open(QIODevice::ReadOnly))
        qFatal("Cannot open server.crt");

    if (!keyFile.open(QIODevice::ReadOnly))
        qFatal("Cannot open server.key");

    server_cert = QSslCertificate(certFile.readAll(), QSsl::Pem);
    server_key  = QSslKey(keyFile.readAll(), QSsl::Rsa, QSsl::Pem);

    if (server_cert.isNull() || server_key.isNull())
        qFatal("Certificate or key is null");

    qDebug() << "Certificate loaded OK";
    listen(QHostAddress::Any, 1234);

    UserBase = new UserDataBase();
}

void Server::incomingConnection(qintptr socketDescriptor)
{

    QSslSocket *userSocket = new QSslSocket(this);

    if (!userSocket->setSocketDescriptor(socketDescriptor)) {
        userSocket->deleteLater();
        return;
    }

    connect(userSocket, &QSslSocket::encryptedBytesWritten, this, [](qint64 bytes){
        qDebug() << "Encrypted bytes sent:" << bytes;
    });

    userSocket->setLocalCertificate(server_cert);
    userSocket->setPrivateKey(server_key);
    userSocket->setPeerVerifyMode(QSslSocket::VerifyNone);

    connect(userSocket, &QSslSocket::encrypted, this, [userSocket]() {
        qDebug() << "[Server] SSL handshake complete";
    });

    connect(userSocket, &QSslSocket::readyRead,this, &Server::readClientData);


    connect(userSocket, &QSslSocket::sslErrors,
            userSocket, [userSocket](const QList<QSslError>&){
                userSocket->ignoreSslErrors();
            });

    connect(userSocket, &QSslSocket::disconnected,this, &Server::userDisconected);

    userSocket->startServerEncryption();
    qDebug() << "[Server]" << userSocket << "connected";
}

void Server::userDisconected()
{
    QSslSocket *userSocketSender = qobject_cast<QSslSocket*>(sender());
    userSockets.remove(userSocketSender);
    userSocketSender->deleteLater();
    onlineUsersIds[authorizedUsers[userSocketSender]] = "offline";

    sendUserStatus(authorizedUsers[userSocketSender]);

    authorizedUsers.remove(userSocketSender);
    qDebug() << "[SERVER]" << "user disconnecting";

}


void Server::readClientData()
{
    QSslSocket *userSenderSocket = qobject_cast<QSslSocket*>(sender());
    if(!userSenderSocket) return;

    buffer.append(userSenderSocket->readAll());

    while(true)
    {
        if(receivingFile)
        {
            qint64 bytesNeeded = expectedFileSize - receivedFileSize;

            if(buffer.size() < bytesNeeded)
            {
                outputFile.write(buffer);
                receivedFileSize += buffer.size();
                buffer.clear();
                return;
            }

            QByteArray filePart = buffer.left(bytesNeeded);
            outputFile.write(filePart);

            buffer.remove(0, bytesNeeded);

            outputFile.close();
            receivingFile = false;

            qDebug() << "File received!";
            if(!receivingFile) sendFile(receivedFileName, authorizedUsers[userSenderSocket], reciverFileId);
        }
        else
        {
            int index = buffer.indexOf('\n');
            if(index == -1) return;

            QByteArray header = buffer.left(index);
            buffer.remove(0, index + 1);

            QString userMessage = QString::fromUtf8(header);
            qDebug() << "[Server] message from user " << userMessage;
            if(userMessage.startsWith("FILE"))
            {
                QStringList parts = userMessage.split("|");
                expectedFileSize = parts[1].toLongLong();

                receivedFileSize = 0;
                receivedFileName = parts[2];
                reciverFileId = parts[4].toInt();
                qDebug() << "[Server] reciverFileId" << reciverFileId;
                outputFile.setFileName(receivedFileName);
                outputFile.open(QIODevice::WriteOnly);

                receivingFile = true;

                qDebug() << "Start receiving file:" << expectedFileSize;
            }

            else if(userMessage.startsWith("AUT"))
            {
                QStringList splitetedUserData = userMessage.split("|");
                qDebug() << "[Server] "  << splitetedUserData[1] << splitetedUserData[2];

                if(UserBase->auntificate(splitetedUserData[1],splitetedUserData[2] ))
                {
                    QString userId = UserBase->getUserId(splitetedUserData[1]);

                    authorizedUsers[userSenderSocket] = userId.toInt();
                    onlineUsersIds[userId.toInt()] = "online";
                    sendAuthMessage(userSenderSocket, userId, true);

                    //sendUserFullName(userSenderSocket); // Remove useless method
                    //sendUserStatus(userId.toInt());
                }
                else
                {
                    sendAuthMessage(userSenderSocket, nullptr,false);
                }
            }
            else if(userMessage.startsWith("MSG"))
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
            else if(userMessage.startsWith("CHAT"))
            {
                QStringList usersIdsSplitedData = userMessage.split("|");

                int sender = usersIdsSplitedData[1].toInt();
                int reciver = usersIdsSplitedData[2].toInt();
                int gettedUsersChatId = UserBase->getPrivateChatIdBetweenUsers(sender, reciver);
                qDebug() << "gettedUsersChatId " << gettedUsersChatId;
                QByteArray usersMessagesInChat = UserBase->getMessages(gettedUsersChatId);
                sendChatData(userSenderSocket, usersMessagesInChat);
            }
            else if(userMessage.startsWith("GCHAT"))
            {
                QStringList chatIdSplitedData = userMessage.split("|");
                QString messages = "GCHAT|";
                userSenderSocket->write(messages.toUtf8() + UserBase->getGroupMessages(chatIdSplitedData[1].toInt()) + '\t');
                userSenderSocket->flush();
            }
            else if(userMessage.startsWith("CREATE"))
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
            else if(userMessage.startsWith("REG"))
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
    }
}

void Server::broadcastPrivateMessage(int senderId, int reciverId,QString message )
{
    QString fullMessage = "MSG|" + QString::number(senderId)  + "|" + message;

    for(auto socketIter = authorizedUsers.begin(); socketIter != authorizedUsers.end(); ++socketIter )
    {
        int userId = socketIter.value();
        QSslSocket* socket = socketIter.key();

        if(userId != reciverId)
            continue;

        socket->write(fullMessage.toUtf8() + '\t');
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
        QSslSocket* socket = socketIter.key();

        if(!chatUsers.contains(userId) || senderId == userId)
            continue;


        QString message = "GMSG|" + QString::number(senderId) + "|"+ QString::number(chatId)+ "|"+ text;
        socket->write(message.toUtf8()+ '\t');
        socket->flush();
    }
}

void Server::broadcastNewGroupChat(int chatId, const QString &chatName, const QList<int> &userIds)
{
    QString message = "CREATE|" + chatName + "|" + QString::number(chatId);

    for(auto socketIter = authorizedUsers.begin(); socketIter != authorizedUsers.end(); ++socketIter )
    {
        int userId = socketIter.value();
        QSslSocket* socket = socketIter.key();

        if(!userIds.contains(userId))
            continue;

        socket->write(message.toUtf8()+ '\t');
        socket->flush();
    }
}

void Server::sendFile(const QString &filename, int senderId,  int reciverId)
{
    //QString filename = "received_file";
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly)) return;


    // create QMap<int, QSslSocket> later
    QSslSocket* socketReciver = nullptr;

    for(auto it = authorizedUsers.begin(); it != authorizedUsers.end(); ++it)
    {
        if(it.value() == reciverId) {
            socketReciver = it.key(); // fix socket it is not needed userSocket
            qDebug() << "socket:" << it.key() << "userId:" << it.value();

            break;
        }
    }


    if(!socketReciver)
    {
        qDebug() << "Receiver not found";
        return;
    }


    qint64 fileSize = file.size();
    QByteArray header = "FILE|" + QByteArray::number(fileSize) +"|" +filename.toUtf8() + "|" + QString::number(senderId).toUtf8() +'\t';
    socketReciver->write(header);

    const qint64 chunkSize = 64 * 1024;
    while(!file.atEnd())
    {
        QByteArray chunk = file.read(chunkSize);
        socketReciver->write(chunk);
        socketReciver->flush();
    }

    file.close();
}

void Server::sendAuthMessage(QSslSocket* userAutSocket, QString userId, bool isAauthenticated)
{
    QString autMessage = "AUT|";

    /*
    if(isAauthenticated)
    {
        autMessage += "Success|" + userId;
        //QString fullNameIdentifier = "|FullName|";

        userAutSocket->write(autMessage.toUtf8());
        userAutSocket->flush();
    }
    else
    {
        autMessage+="Error";
        userAutSocket->write(autMessage.toUtf8());
        userAutSocket->flush();
    }
*/



    if(isAauthenticated)
    {
        autMessage += "Success|" + userId + "|FullName|";
        //QString fullNameIdentifier = "|FullName|";
        userAutSocket->write(autMessage.toUtf8() + UserBase->userDataForSending(authorizedUsers[userAutSocket], onlineUsersIds)+ '\t');
        userAutSocket->flush();
        //sendUserFullName(userAutSocket);
    }
    else
    {
        autMessage+="Error";
        userAutSocket->write(autMessage.toUtf8()+ '\t');
        userAutSocket->flush();
    }


}

void Server::sendUserFullName(QSslSocket* userFullNameSocket)
{
    QString fullNameIdentifier = "|FullName|";
    userFullNameSocket->write(fullNameIdentifier.toUtf8() + UserBase->userDataForSending(authorizedUsers[userFullNameSocket], onlineUsersIds) + '\t');
    userFullNameSocket->flush();
}

void Server::sendUserStatus(int userId)
{
    QString userSatatus = "STAT|" + QString::number(userId) +"|"+onlineUsersIds[userId];
    for(auto socketIter = authorizedUsers.begin(); socketIter != authorizedUsers.end(); ++socketIter)
    {
        QSslSocket* userSocket = socketIter.key();
        int id = socketIter.value();
        if(id == userId)
            continue;

        userSocket->write(userSatatus.toUtf8()+ '\t');
        userSocket->flush();
    }
}



void Server::sendChatData(QSslSocket* userSocketForChatHistory, QByteArray chatData)
{
    QString allMessages = "CHAT|";
    userSocketForChatHistory->write(allMessages.toUtf8() + chatData+ '\t');
    userSocketForChatHistory->flush();
}

