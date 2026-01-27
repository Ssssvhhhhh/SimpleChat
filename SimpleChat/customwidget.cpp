#include "customwidget.h"
#include "ui_customwidget.h"




CustomWidget::CustomWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CustomWidget)
{
    ui->setupUi(this);
}

CustomWidget::CustomWidget(QWidget *parent,QTcpSocket* mainSocket, QString serverName, QString serverIP, int serverPort)
    : QWidget(parent), ui(new Ui::CustomWidget)
{
    ui->setupUi(this);
    ui->labelServerName->setText(serverName);
    serverStruct.serverNameS = serverName;
    serverStruct.serverIPS = serverIP;
    serverStruct.serverPortS = serverPort;

    userSocketFromMain = mainSocket;
}

void CustomWidget::connectToCurrentServer(QTcpSocket* userSocket)
{
    userSocket->connectToHost(serverStruct.serverIPS, serverStruct.serverPortS);
    !userSocket->waitForConnected(3000) ? qDebug() << "[CLIENT] " << "Connection error": qDebug() << "[CLIENT] " << "Connection Succses";
}



CustomWidget::~CustomWidget()
{
    delete ui;
}


void CustomWidget::on_pushButtonConnect_clicked()
{
    connectToCurrentServer(userSocketFromMain);
}

