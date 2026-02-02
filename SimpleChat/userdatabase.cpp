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

QByteArray UserDataBase::userNameSernameForSending()
{
    QSqlQuery userDataQuery(usersBase);

    userDataQuery.prepare("SELECT name, sername, id FROM UserBase");
    if (!userDataQuery.exec())
    {
        qDebug() << "[DataBase] user name and sername selected error: " << userDataQuery.lastError().text();
    }

    QJsonArray userDataJson;
    while (userDataQuery.next())
    {
        QJsonObject objectUserData;
        objectUserData["full_name"] = userDataQuery.value("name").toString() + " " + userDataQuery.value("sername").toString() + "&" + userDataQuery.value("id").toString();
        userDataJson.append(objectUserData);
    }
    QJsonDocument usersDataJsonDoc(userDataJson);
    qDebug() << "[DataBase]" << "users" << usersDataJsonDoc;

    return usersDataJsonDoc.toJson();

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

    if (!createdChatQuery.exec("INSERT INTO chats DEFAULT VALUES")) {
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

int UserDataBase::getChatIdBetweenUsers(int user1, int user2)
{
    QSqlQuery getChatIdBetweenUsersQuery(usersBase);
    getChatIdBetweenUsersQuery.prepare(        "SELECT cu1.chat_id "
                                       "FROM chat_users cu1 "
                                       "JOIN chat_users cu2 ON cu1.chat_id = cu2.chat_id "
                                       "WHERE cu1.user_id = ? AND cu2.user_id = ?");

    getChatIdBetweenUsersQuery.addBindValue(user1);
    getChatIdBetweenUsersQuery.addBindValue(user2);

    if (!getChatIdBetweenUsersQuery.exec())
    {
        qDebug() << "[DataBase] getChatId error:" << getChatIdBetweenUsersQuery.lastError().text();
        return -1;
    }
    if(getChatIdBetweenUsersQuery.next())
    {
        return getChatIdBetweenUsersQuery.value(0).toInt();
    }

    return -1;

}

bool UserDataBase::saveMessageToBase(int senderId, int receiverId, const QString &text)
{
    usersBase.transaction();

    int chatId = getChatIdBetweenUsers(senderId, receiverId);

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

QByteArray UserDataBase::getMessages(int chatId)
{
    QSqlQuery getMessageQuery(usersBase);

    getMessageQuery.prepare(
        "SELECT sender_id, text, created_at "
        "FROM messages "
        "WHERE chat_id = ? "
        "ORDER BY created_at"
        );
    getMessageQuery.addBindValue(chatId);

    if (!getMessageQuery.exec()) {
        qDebug() << "[DataBase] message not get:" << getMessageQuery.lastError().text();
    }

    QJsonArray chatMessagesData;


    while (getMessageQuery.next())
    {
        QJsonObject objectChatData;
        objectChatData["message"] =  getMessageQuery.value(2).toString() + " " + getMessageQuery.value(0).toString() + " " + getMessageQuery.value(1).toString();
        chatMessagesData.append(objectChatData);
    }

    QJsonDocument chatDataJson(chatMessagesData);
    return chatDataJson.toJson();
}

