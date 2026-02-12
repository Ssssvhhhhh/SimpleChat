#include "userdatabase.h"

UserDataBase::UserDataBase()
{
    usersBase = QSqlDatabase::addDatabase("QSQLITE", "UserDB");
    usersBase.setDatabaseName("SQLUserDataBase.db");

    if (!usersBase.open())
    {
        qDebug() << "[DataBase] OPEN ERROR:" << usersBase.lastError().text();
    } else {
        qDebug() << "[DataBase] Database opened";
    }
    qDebug() << "DB FILE:" << usersBase.databaseName();
    qDebug() << "Tables:" << usersBase.tables();

}

void UserDataBase::addUserInDataBase(userData data)
{

    QSqlQuery addQuery(usersBase);

    addQuery.prepare(
        "INSERT INTO UserBase (login, userpassword, name, sername, email) "
        "VALUES (?, ?, ?, ?, ?)"
        );

    addQuery.addBindValue(data.login);
    addQuery.addBindValue(data.password);
    addQuery.addBindValue(data.name);
    addQuery.addBindValue(data.sername);
    addQuery.addBindValue(data.email);

    if (!addQuery.exec())
    {
        qDebug() << "[DataBase] user addition error:" << addQuery.lastError().text();
    }
    else
    {
        qDebug() << "[DataBase] user added OK";
    }
}

QByteArray UserDataBase::userDataForSending(int userId, QMap<int, QString> onlineUsersIds)
{
    QJsonObject rootObject;

    QSqlQuery userDataQuery(usersBase);
    userDataQuery.prepare("SELECT id, name, sername FROM UserBase");

    if (!userDataQuery.exec())
    {
        qDebug() << "[DataBase] user select error:" << userDataQuery.lastError().text();
    }

    QJsonArray usersArray;

    while (userDataQuery.next())
    {
        QJsonObject userObj;
        userObj["id"] = userDataQuery.value("id").toInt();
        userObj["name"] = userDataQuery.value("name").toString();
        userObj["sername"] = userDataQuery.value("sername").toString();
        if(onlineUsersIds[userDataQuery.value("id").toInt()] == "online")
        {
            userObj["status"] = "online";
        }
        else
        {
            userObj["status"] = "offline";
        }
        usersArray.append(userObj);
    }

    rootObject["users"] = usersArray;


    QSqlQuery chatsQuery(usersBase);
    chatsQuery.prepare(R"(
        SELECT chats.id, chats.name, chats.type
        FROM chats
        JOIN chat_users ON chats.id = chat_users.chat_id
        WHERE chat_users.user_id = ?
    )");

    chatsQuery.addBindValue(userId);

    if (!chatsQuery.exec())
    {
        qDebug() << "[DataBase] chats select error:"
                 << chatsQuery.lastError().text();
    }

    QJsonArray chatsArray;

    while (chatsQuery.next())
    {
        QJsonObject chatObj;
        chatObj["chat_id"] = chatsQuery.value("id").toInt();
        chatObj["name"] = chatsQuery.value("name").toString();
        chatObj["type"] = chatsQuery.value("type").toString();

        if (chatObj["type"] == "private") // костыль
            continue;

        chatsArray.append(chatObj);
    }

    rootObject["chats"] = chatsArray;


    QJsonDocument doc(rootObject);
    return doc.toJson();
}


bool UserDataBase::auntificate(const QString &login, const QString &password)
{
    QSqlQuery authenticateQuery(usersBase);

    authenticateQuery.prepare(
        "SELECT 1 FROM UserBase "
        "WHERE login = :Login AND userpassword = :Userpassword");

    authenticateQuery.bindValue(":Login", login);
    authenticateQuery.bindValue(":Userpassword", password);

    if (!authenticateQuery.exec())
    {
        qDebug() << "[DataBase] user auntificate error:" << authenticateQuery.lastError().text();
    }


    if(authenticateQuery.next())
    {
        qDebug() << "[DataBase] user auntificated";
        return true;
    }
    else
    {
        qDebug() << "[DataBase] user not auntificated";
        return false;
    }

}

QString UserDataBase::getUserId(const QString &login)
{
    QSqlQuery getIdQuery(usersBase);
    getIdQuery.prepare("SELECT id FROM UserBase WHERE login = :Login");
    getIdQuery.bindValue(":Login", login);
    if (!getIdQuery.exec())
    {
        qDebug() << "[DataBase] user auntificate error:" << getIdQuery.lastError().text();
    }

    if(getIdQuery.next())
    {
        return getIdQuery.value("id").toString();
    }
}

int UserDataBase::createChat()
{
    QSqlQuery createdChatQuery(usersBase);

    if (!createdChatQuery.exec("INSERT INTO chats (type) VALUES ('private')")) {
        qDebug() << "[DataBase]"  << "chat creating error " << createdChatQuery.lastError().text();
        return -1;
    }

    return  createdChatQuery.lastInsertId().toInt();

}

bool UserDataBase::addMessage(int chatId, int senderId, const QString &text)
{
    QSqlQuery addMessageQuery(usersBase);
    addMessageQuery.prepare("INSERT INTO messages (chat_id, sender_id, text) VALUES (?, ?, ?)");
    addMessageQuery.addBindValue(chatId);
    addMessageQuery.addBindValue(senderId);
    addMessageQuery.addBindValue(text);

    if(!addMessageQuery.exec())
    {
        qDebug() << "[DataBase]"  << "message not added " << addMessageQuery.lastError().text();
        return false;
    }
    return true;
}

bool UserDataBase::addUserToChat(int chatId, int userId)
{
    QSqlQuery addUserInChatQuery(usersBase);
    addUserInChatQuery.prepare("INSERT INTO chat_users (chat_id, user_id) VALUES (?, ?)");
    addUserInChatQuery.addBindValue(chatId);
    addUserInChatQuery.addBindValue(userId);

    if (!addUserInChatQuery.exec())
    {
        qDebug() << "[DataBase] addUserToChat error:" << addUserInChatQuery.lastError().text();
        return false;
    }
    return true;
}

int UserDataBase::getPrivateChatIdBetweenUsers(int user1, int user2)
{
    QSqlQuery getPrivateChatIdQuery(usersBase);

    QString sql =
        "SELECT c.id "
        "FROM chats c "
        "JOIN chat_users cu1 ON c.id = cu1.chat_id "
        "JOIN chat_users cu2 ON c.id = cu2.chat_id "
        "WHERE c.type = 'private' "
        "AND cu1.user_id = ? "
        "AND cu2.user_id = ?";

    getPrivateChatIdQuery.prepare(sql);
    getPrivateChatIdQuery.addBindValue(user1);
    getPrivateChatIdQuery.addBindValue(user2);

    if (!getPrivateChatIdQuery.exec())
    {
        qDebug() << "getPrivateChatId error:" << getPrivateChatIdQuery.lastError().text();
        return -1;
    }

    if (getPrivateChatIdQuery.next())
        return getPrivateChatIdQuery.value(0).toInt();

    return -1;
}


bool UserDataBase::saveMessageToBase(int senderId, int receiverId, const QString &text)
{
    usersBase.transaction();

    int chatId = getPrivateChatIdBetweenUsers(senderId, receiverId);

    if (chatId == -1)
    {
        chatId = createChat();
        if (chatId == -1)
        {
            usersBase.rollback();
            return false;
        }

        addUserToChat(chatId, senderId);
        addUserToChat(chatId, receiverId);
    }

    if (!addMessage(chatId, senderId, text))
    {
        usersBase.rollback();
        return false;
    }

    usersBase.commit();
    return true;
}

bool UserDataBase::saveGroupMessage(int senderId, int chatId, const QString &text)
{
    return addMessage(chatId, senderId, text);
}

QByteArray UserDataBase::getMessages(int chatId)
{
    QSqlQuery query(usersBase);

    QString sql =
        "SELECT u.name, u.sername, m.text "
        "FROM messages AS m "
        "INNER JOIN UserBase AS u ON m.sender_id = u.id "
        "WHERE m.chat_id = ? "
        "ORDER BY m.id ASC";

    if (!query.prepare(sql)) {
        qDebug() << "[DataBase] Prepare error:" << query.lastError().text();
        return QByteArray();
    }

    query.addBindValue(chatId);

    if (!query.exec()) {
        qDebug() << "[DataBase] Exec error:" << query.lastError().text();
        return QByteArray();
    }

    QJsonArray messagesArray;

    while (query.next())
    {
        QJsonObject messageObject;

        QString sender =
            query.value(0).toString() + " " +
            query.value(1).toString();

        messageObject["sender"] = sender;
        messageObject["text"] = query.value(2).toString();

        messagesArray.append(messageObject);
    }

    QJsonDocument doc(messagesArray);
    return doc.toJson(QJsonDocument::Compact);
}



int UserDataBase::createGroupChat(const QList<int> &userIds, const QString &groupName)
{
    QSqlQuery createGroupQuery(usersBase);
    createGroupQuery.prepare("INSERT INTO chats (type, name) VALUES ('group', ?)");
    createGroupQuery.addBindValue(groupName);

    if(!createGroupQuery.exec())
    {
        qDebug() << "[DataBase] group not created" << createGroupQuery.lastError().text();
        return -1;
    }

    int chatId = createGroupQuery.lastInsertId().toInt();

    for (int userId : userIds)
    {
        QSqlQuery addUserInGroupQuery(usersBase);
        addUserInGroupQuery.prepare("INSERT INTO chat_users (chat_id, user_id) VALUES (?, ?)");
        addUserInGroupQuery.addBindValue(chatId);
        addUserInGroupQuery.addBindValue(userId);

        if(!addUserInGroupQuery.exec())
        {
            qDebug() << "[DataBase] addUserToChat error:"
                     << addUserInGroupQuery.lastError().text();
        }
    }

    return chatId;
}


QList<int> UserDataBase::getChatUsers(int chatId)
{
    QList<int> usersIds;
    QSqlQuery getUsersIdsQuery(usersBase);

    getUsersIdsQuery.prepare("SELECT user_id FROM chat_users WHERE chat_id = ?");
    getUsersIdsQuery.addBindValue(chatId);

    if(!getUsersIdsQuery.exec())
    {
        qDebug() << "[DataBase] get group ids error:" << getUsersIdsQuery.lastError().text();
        return usersIds;
    }
    while(getUsersIdsQuery.next())
    {
        usersIds.append(getUsersIdsQuery.value(0).toInt());
    }

    return usersIds;
}

QByteArray UserDataBase::getGroupMessages(int chatId)
{
    QSqlQuery query(usersBase);

    query.prepare(R"(
        SELECT
            m.id AS message_id,
            u.name || ' ' || u.sername AS sender_name,
            m.text,
            m.created_at
        FROM messages m
        JOIN UserBase u ON m.sender_id = u.id
        WHERE m.chat_id = ?
        ORDER BY m.created_at ASC
    )");

    query.addBindValue(chatId);

    if (!query.exec())
    {
        qDebug() << "[DataBase] getGroupMessages error:" << query.lastError().text();
        return QByteArray();
    }

    QJsonArray messagesArray;

    while (query.next())
    {
        QJsonObject messageObject;

        messageObject["sender_name"] = query.value("sender_name").toString();
        messageObject["message"] = query.value("text").toString();

        messagesArray.append(messageObject);
    }

    QJsonDocument doc(messagesArray);
    return doc.toJson(QJsonDocument::Indented);
}
