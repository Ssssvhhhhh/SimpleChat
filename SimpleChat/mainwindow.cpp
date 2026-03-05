#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , userSocket(new QSslSocket(this))
{
    ui->setupUi(this);
    //userSocket->connectToHost("127.0.0.1", 1234);

    connect(userSocket, &QSslSocket::readyRead, this, &MainWindow::readServerResponse);
    //!userSocket->waitForConnected(3000) ? qDebug() << "[CLIENT] " << "Connection error": qDebug() << "[CLIENT] " << "Connection Succses";
    connect(userSocket, &QSslSocket::connected, this, &MainWindow::showConnectionLabel);

    ui->labelError->hide();
    ui->labelError_2->hide();
    ui->labelError_3->hide();
    ui->labelError_4->hide();

    ui->treeWidget->setColumnCount(1);
    ui->treeWidget->setHeaderHidden(true);
    ui->pushButtonCreateGroup->hide();
    ui->tableWidgetGroup->setColumnCount(1);
    ui->tableWidgetGroup->horizontalHeader()->hide();
    ui->treeWidget->setFocusPolicy(Qt::NoFocus);

    setWindowTitle("Simple Chat");
    setWindowIcon(QIcon("images/appIcon.png"));





    connect(userSocket, &QSslSocket::connected, this, [](){
        qDebug() << "TCP connected";
    });

    connect(userSocket, &QSslSocket::encrypted, this, [](){
        qDebug() << "SSL handshake success";
    });

    connect(userSocket, &QSslSocket::readyRead,
            this, &MainWindow::readServerResponse);

    userSocket->setPeerVerifyMode(QSslSocket::VerifyNone);
    userSocket->ignoreSslErrors();

}

MainWindow::~MainWindow()
{
    delete ui;
    userSocket->disconnectFromHost();
    qDebug() << "[Client]" << " was destruct";
}

void MainWindow::opencloseTab()
{
    if(isTabVisible)
    {
        ui->frameTab->hide();
        isTabVisible = false;
    }
    else
    {
        ui->frameTab->show();
        isTabVisible = true;
    }
}

void MainWindow::openServerCreationSettings()
{
    ui->stackedWidget->setCurrentIndex(0);
}

/*
void MainWindow::openCloseServerUsers()
{
    //ui->stackedWidgetTab->setCurrentIndex(1);
}
*/
void MainWindow::readServerResponse()
{
    QByteArray responseServerData = userSocket->readAll();
    QString messageFromServer = QString::fromUtf8(responseServerData);
    qDebug() << "[Client] " << "message from server " << messageFromServer;
    if(messageFromServer.startsWith("AUT"))
    {
        QStringList splitedDataFromServer = messageFromServer.split("|");
        if(splitedDataFromServer[1] == "Success")
        {
            ui->stackedWidget->setCurrentIndex(2);
            ui->pushButtonCreateGroup->show();
            userId = splitedDataFromServer[2].toInt();
            isAuthorized = true;
            addUserFullNameInTab(splitedDataFromServer[4]);

            qDebug() << "[Client] " << "user id" << userId;
        }
    }
    if(messageFromServer.startsWith("MSG"))
    {
        QStringList splitedMessageFromServer = messageFromServer.split("|");
        if(currentChatOrUserId == splitedMessageFromServer[1].toInt())
        {
            ui->textBrowserChat->append(usersNamesAndId[splitedMessageFromServer[1].toInt()] + ":" + splitedMessageFromServer[2]);
        }
    }
    if(messageFromServer.startsWith("GMSG"))
    {
        QStringList splitedGroupMessageFromServer = messageFromServer.split("|");
        if(currentChatOrUserId == splitedGroupMessageFromServer[2].toInt())
        {
            ui->textBrowserChat->append(usersNamesAndId[splitedGroupMessageFromServer[1].toInt()] + ":" + splitedGroupMessageFromServer[3]);
        }
    }
    if(messageFromServer.startsWith("GCHAT"))
    {
        QStringList splitedGroupMessagesFromServer = messageFromServer.split("|");
        loadGroupChatMessages(splitedGroupMessagesFromServer[1]);
    }
    if(messageFromServer.startsWith("CHAT"))
    {
        QStringList splitedChatMessages = messageFromServer.split("|");
        loadChatMessages(splitedChatMessages[1]);
    }

    if(messageFromServer.startsWith("CREATE"))
    {
        QStringList splitedGroupData = messageFromServer.split("|");
        QTreeWidgetItem* groupNameItem = new QTreeWidgetItem(serverItem);
        userWidget = new UserStatusWidget(nullptr, splitedGroupData[1], splitedGroupData[2].toInt(), "group", "");
        ui->treeWidget->setItemWidget(groupNameItem,0,userWidget);
        connect(userWidget, &UserStatusWidget::userNameClicked, this, &MainWindow::onUserNameRecevied);
    }
    if(messageFromServer.startsWith("STAT"))
    {
        QStringList splitedStatusData = messageFromServer.split("|");
        int id = splitedStatusData[1].toInt();
        QString status = splitedStatusData[2];
        changeUserStatus(id, status);
    }

}


void MainWindow::addServer()
{
    QString serverName = ui->lineEditServerName->text().trimmed();
    QString serverIP = ui->lineEditServerIP->text().trimmed();
    QString serverPort = ui->lineEditServerPort->text().trimmed();

    if(!serverName.isEmpty() && !serverIP.isEmpty() && !serverPort.isEmpty())
    {
        serverItem = new QTreeWidgetItem(ui->treeWidget);
        serverWidget = new CustomWidget(nullptr, userSocket, serverName,serverIP,serverPort.toInt());
        ui->treeWidget->setItemWidget(serverItem, 0, serverWidget);
    }
    else
    {
        labelError(ui->labelError_4);
    }
}

void MainWindow::sendAuthorizationData()
{
    QString login = ui->lineEditLogin->text().trimmed();
    QString password = ui->lineEditPassword->text().trimmed();

    if(!login.isEmpty() && !password.isEmpty())
    {
        QString authorizationData = "AUT|" + login + "|" + password;
        userSocket->write(authorizationData.toUtf8());
        qDebug() << "[user] data was sent";
        userSocket->flush();
    }
    else
    {
        labelError(ui->labelError);
    }
}

void MainWindow::addUserFullNameInTab(const QString& usersFullName)
{
    QJsonDocument usersNameDoc = QJsonDocument::fromJson(usersFullName.toUtf8());
    QJsonObject rootObj = usersNameDoc.object();
    QJsonArray usersNameArray = rootObj["users"].toArray();

    for(const QJsonValue& value : usersNameArray)
    {
        QJsonObject userObj = value.toObject();
        int id = userObj["id"].toInt();



        QString name = userObj["name"].toString();
        QString sername = userObj["sername"].toString();
        QString status = userObj["status"].toString();
        QString fullName = name + " " + sername;
        usersNamesAndId[id] = fullName;

        if(userId == id)
            continue;

        QTreeWidgetItem* userNameItem = new QTreeWidgetItem(serverItem);
        userWidget = new UserStatusWidget(nullptr, fullName, id, "private", status);
        ui->treeWidget->setItemWidget(userNameItem, 0 , userWidget);

        connect(userWidget, &UserStatusWidget::userNameClicked, this, &MainWindow::onUserNameRecevied);
        connect(userWidget, &UserStatusWidget::userIdForGroup, this, &MainWindow::addUserInGroup);
    }


    QJsonArray groupArray = rootObj["chats"].toArray();

    for(const QJsonValue& value : groupArray)
    {
        QJsonObject chatObj = value.toObject();
        int chatId = chatObj["chat_id"].toInt();
        QString groupName = chatObj["name"].toString();
        QString type = chatObj["type"].toString();

        usersNamesAndId[chatId] = groupName;

        QTreeWidgetItem* groupNameItem = new QTreeWidgetItem(serverItem);
        userWidget = new UserStatusWidget(nullptr, groupName, chatId, "group", "");
        ui->treeWidget->setItemWidget(groupNameItem,0,userWidget);
        connect(userWidget, &UserStatusWidget::userNameClicked, this, &MainWindow::onUserNameRecevied);
    }

}

void MainWindow::sendMessageToCurrentUser()
{
    QString message = ui->lineEditMessage->text();
    QString fullMessage = "MSG|" +  currentChatType +"|"+ QString::number(userId) + "|" + QString::number(currentChatOrUserId) + "|" + message;
    userSocket->write(fullMessage.toUtf8());
    userSocket->flush();
    ui->textBrowserChat->append(usersNamesAndId[userId] + ":" + message);
    ui->lineEditMessage->clear();
}

void MainWindow::onUserNameRecevied(int id, QString type)
{
    currentChatOrUserId = id;
    currentChatType = type;

    if(currentChatType == "private")
    {
        getAllMessagesInChat(userId, currentChatOrUserId);
    }
    else
    {
        getAllMessagesInGroupChat(currentChatOrUserId);
    }
    qDebug()<<"[Client]" << "current user id" << currentChatOrUserId << currentChatType ;
    ui->textBrowserChat->clear();
}

void MainWindow::getAllMessagesInGroupChat(int chatId)
{
    QString groupMessagesRequest = "GCHAT|" + QString::number(chatId);
    userSocket->write(groupMessagesRequest.toUtf8());
    userSocket->flush();
}



void MainWindow::getAllMessagesInChat(int senderId, int reciverId)
{
    QString usersIds = "CHAT|"+QString::number(senderId) + "|" + QString::number(reciverId);
    userSocket->write(usersIds.toUtf8());
    userSocket->flush();
}

void MainWindow::loadChatMessages(const QString &chatMessages)
{
    QJsonDocument chatMessagesDoc = QJsonDocument::fromJson(chatMessages.toUtf8());
    QJsonArray chatMessagesArray = chatMessagesDoc.array();

    for(const QJsonValue& value : chatMessagesArray)
    {
        QJsonObject chatMessagsObj = value.toObject();
        QString name = chatMessagsObj["sender"].toString();
        QString text = chatMessagsObj["text"].toString();

        ui->textBrowserChat->append(name + ": " + text);
    }
}

void MainWindow::loadGroupChatMessages(const QString &groupMessages)
{
    QJsonDocument usersMessagesDoc = QJsonDocument::fromJson(groupMessages.toUtf8());
    QJsonArray groupChatMessagesArray = usersMessagesDoc.array();
    for(const QJsonValue& value : groupChatMessagesArray)
    {
        QJsonObject groupChatMessagsObj = value.toObject();
        QString name = groupChatMessagsObj["sender_name"].toString();
        QString text = groupChatMessagsObj["message"].toString();
        ui->textBrowserChat->append(name + ": " + text);
    }
}

void MainWindow::addUserInGroup(int userId)
{
    usersIdsForGroup.append(userId);
    int row = ui->tableWidgetGroup->rowCount();
    ui->tableWidgetGroup->insertRow(row);
    ui->tableWidgetGroup->setItem(row, 0, new QTableWidgetItem(usersNamesAndId[userId]));
}

void MainWindow::sendDataToCreateGroupChat(QList<int> usersIdsForGroup)
{
    QString groupName = ui->lineEditGroupName->text().trimmed();

    if(!groupName.isEmpty())
    {
        QString GroupRequest = "CREATE|" + groupName + "|";
        GroupRequest.append("&"+ QString::number(userId));
        for(int groupIter : usersIdsForGroup)
        {
            GroupRequest.append("&");
            GroupRequest.append(QString::number(groupIter));
        }
        userSocket->write(GroupRequest.toUtf8());
        userSocket->flush();
    }
    else
    {
        labelError(ui->labelError_3);
    }

}

void MainWindow::changeUserStatus(int userId, QString status)
{
    QList<UserStatusWidget*> userWidgets = findChildren<UserStatusWidget*>();
    for(UserStatusWidget* widgetIter : userWidgets)
    {
        if(widgetIter->getId() == userId)
        {
            if(widgetIter->getStatus() == "offline")
            {
                widgetIter->setStatus(true);
                break;
            }
            else
            {
                widgetIter->setStatus(false);
                break;
            }
        }

    }
}

void MainWindow::showAddButtonOnWidgets(bool& isShow)
{
    QList<UserStatusWidget*> userWidgets = findChildren<UserStatusWidget*>();
    for(UserStatusWidget* widgetIter : userWidgets)
    {
        if(widgetIter->getType() == "group" )
            continue;
        if(!isShow)
        {
            widgetIter->showButton();
        }
        else
        {
            widgetIter->hideButton();
        }
    }

    isAddButtonShow = !isAddButtonShow;

}

void MainWindow::sendRegistrationData()
{
    QString name = ui->lineEditName->text().trimmed();
    QString sername = ui->lineEditSername->text().trimmed();
    QString login = ui->lineEditLogin_2->text().trimmed();
    QString password = ui->lineEditPassword_2->text().trimmed();
    QString email = ui->lineEditEmail->text().trimmed();

    if(!name.isEmpty() && !sername.isEmpty() && !login.isEmpty() && !password.isEmpty() && !email.isEmpty())
    {
        QString registrationMessage = "REG|" + name + "|" + sername + "|" + login + "|"+ password + "|" + email;
        userSocket->write(registrationMessage.toUtf8());
        userSocket->flush();
    }
    else
    {
        labelError(ui->labelError_2);
    }


}

void MainWindow::labelError(QLabel *errorLabel)
{
    errorLabel->show();
    QTimer::singleShot(2500, errorLabel, &QLabel::hide);
}

void MainWindow::showConnectionLabel()
{
    ui->labelError_4->setText("Successful connection");
    labelError(ui->labelError_4);
    //ui->labelError_4->setText("ERROR");
}



void MainWindow::on_pushButtonOpenCloseTab_clicked()
{
    opencloseTab();
}

void MainWindow::on_pushButtonAddServer_clicked()
{
    openServerCreationSettings();
}

void MainWindow::on_pushButtonBack_clicked()
{
    isAuthorized ? ui->stackedWidget->setCurrentIndex(2) : ui->stackedWidget->setCurrentIndex(1);

}

void MainWindow::on_pushButtonSend_clicked()
{
    sendMessageToCurrentUser();
}

void MainWindow::on_pushButton_clicked()
{
    addServer();
}

void MainWindow::on_pushButtonAuthorization_clicked()
{
    sendAuthorizationData();
}

void MainWindow::on_pushButtonCreateGroup_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
    showAddButtonOnWidgets(isAddButtonShow);
}


void MainWindow::on_pushButtonSendGroupData_clicked()
{
    sendDataToCreateGroupChat(usersIdsForGroup);
}


void MainWindow::on_pushButtonBackToChats_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    showAddButtonOnWidgets(isAddButtonShow);
}


// For test delete later
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (userSocket) {
        connect(userSocket, &QSslSocket::disconnected, userSocket, &QObject::deleteLater);
        userSocket->disconnectFromHost();
    }
    event->accept();
}



void MainWindow::on_pushButtonReg_clicked()
{
    sendRegistrationData();
}


void MainWindow::on_pushButtonSignUp_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}


void MainWindow::on_pushButtonBck_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

