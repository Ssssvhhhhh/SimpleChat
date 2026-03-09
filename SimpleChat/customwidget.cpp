#include "customwidget.h"
#include "ui_customwidget.h"
#include <qsslsocket.h>




CustomWidget::CustomWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CustomWidget)
{
    ui->setupUi(this);

}

CustomWidget::CustomWidget(QWidget *parent,QSslSocket* mainSocket, QString serverName, QString serverIP, int serverPort)
    : QWidget(parent), ui(new Ui::CustomWidget)
{
    ui->setupUi(this);
    ui->labelServerName->setText(serverName);
    serverStruct.serverNameS = serverName;
    serverStruct.serverIPS = serverIP;
    serverStruct.serverPortS = serverPort;

    userSocketFromMain = mainSocket;

}

void CustomWidget::connectToCurrentServer(QSslSocket* userSocket)
{
    if(userSocket->state() == QAbstractSocket::ConnectedState)
    {
        userSocket->disconnectFromHost();
    }
    else
    {
        userSocket->connectToHostEncrypted(serverStruct.serverIPS, serverStruct.serverPortS);
    }
    /*
    if(!userSocket->waitForConnected(1500))
    {
        QMessageBox::warning(this, "Error", "Connection error");
    }
    */
}



CustomWidget::~CustomWidget()
{
    delete ui;
}

void CustomWidget::on_pushButtonConnect_clicked()
{
    connectToCurrentServer(userSocketFromMain);
}

