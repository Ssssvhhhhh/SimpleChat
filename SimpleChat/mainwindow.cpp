#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , userSocket(new QTcpSocket(this))
{
    ui->setupUi(this);
    //userSocket->connectToHost("127.0.0.1", 1234);

    connect(userSocket, &QTcpSocket::readyRead, this, &MainWindow::readServerResponse);
    //!userSocket->waitForConnected(3000) ? qDebug() << "[CLIENT] " << "Connection error": qDebug() << "[CLIENT] " << "Connection Succses";
    connect(userSocket, &QTcpSocket::connected, this, &MainWindow::openCloseServerUsers);
    /*
    ui->tableWidgetServers->setColumnCount(1);
    ui->tableWidgetServers->setColumnWidth(0, 160);
    ui->tableWidgetServers->verticalHeader()->hide();
    ui->tableWidgetServers->horizontalHeader()->hide();
    */
    ui->treeWidget->setColumnCount(1);
    ui->treeWidget->setHeaderHidden(true);
}

MainWindow::~MainWindow()
{
    delete ui;
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
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::openCloseServerUsers()
{
    //ui->stackedWidgetTab->setCurrentIndex(1);
}

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
            ui->stackedWidget->setCurrentIndex(0);
            addUserFullNameInTab(splitedDataFromServer[4]);
            userId = splitedDataFromServer[2].toInt();
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
    if(messageFromServer.startsWith("CHAT"))
    {
        QStringList splitedChatMessages = messageFromServer.split("|");

        loadChatMessages(splitedChatMessages[1]);
    }

}


void MainWindow::addServer()
{
    QString serverName = ui->lineEditServerName->text();
    QString serverIP = ui->lineEditServerIP->text();
    QString serverPort = ui->lineEditServerPort->text();


    serverItem = new QTreeWidgetItem(ui->treeWidget);
    serverWidget = new CustomWidget(nullptr, userSocket, serverName,serverIP,serverPort.toInt());
    ui->treeWidget->setItemWidget(serverItem, 0, serverWidget);

    /*
    int row = ui->tableWidgetServers->rowCount();
    ui->tableWidgetServers->insertRow(row);
    ui->tableWidgetServers->setRowHeight(row, 50);


    QWidget* cellWidget = new QWidget(ui->tableWidgetServers);
    QHBoxLayout* layout = new QHBoxLayout(cellWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    serverWidget = new CustomWidget(cellWidget, userSocket, serverName,serverIP,serverPort.toInt());
    layout->addWidget(serverWidget);

    ui->tableWidgetServers->setCellWidget(row, 0, cellWidget);
    */

}

void MainWindow::sendAuthorizationData()
{
    QString login = ui->lineEditLogin->text();
    QString password = ui->lineEditPassword->text();

    QString authorizationData = "AUT|" + login + "|" + password;

    userSocket->write(authorizationData.toUtf8());
    userSocket->flush();
}

void MainWindow::addUserFullNameInTab(const QString& usersFullName)
{
    QJsonDocument usersNameDoc = QJsonDocument::fromJson(usersFullName.toUtf8());
    QJsonArray usersNameArray = usersNameDoc.array();

    for(const QJsonValue& value : usersNameArray)
    {
        QJsonObject usersNameObj = value.toObject();
        QString fullName = usersNameObj["full_name"].toString();
        QStringList splitedNameForId = fullName.split("&"); // подумать над сплитом

        usersNamesAndId[splitedNameForId[1].toInt()] = splitedNameForId[0]; //может удалить?

        QTreeWidgetItem* userNameItem = new QTreeWidgetItem(serverItem);
        userWidget = new UserStatusWidget(nullptr, splitedNameForId[0], splitedNameForId[1].toInt());
        ui->treeWidget->setItemWidget(userNameItem,0,userWidget);
        qDebug() << "[Client] " << fullName;

        connect(userWidget, &UserStatusWidget::userNameClicked, this, &MainWindow::onUserNameRecevied);
    }
}

void MainWindow::sendMessageToCurrentUser()
{
    QString message = ui->lineEditMessage->text();
    QString fullMessage = "MSG|"+ QString::number(userId) + "|" + QString::number(currentChatOrUserId) + "|" + message;
    userSocket->write(fullMessage.toUtf8());
    userSocket->flush();
    ui->textBrowserChat->append(usersNamesAndId[userId] + ":" + message);
    ui->lineEditMessage->clear();
}

void MainWindow::onUserNameRecevied(int id)
{
    currentChatOrUserId = id;
    getAllMessagesInChat(userId, currentChatOrUserId);
    qDebug()<<"[Client]" << "current user id" << currentChatOrUserId;
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
        QString message = chatMessagsObj["message"].toString(); // Сделать чтобы вместо id было имя пользлватетля!!
        ui->textBrowserChat->append(message);
    }
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
    isAuthorized ? ui->stackedWidget->setCurrentIndex(0) : ui->stackedWidget->setCurrentIndex(3);
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

