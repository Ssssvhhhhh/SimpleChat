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
    ui->tableWidgetServers->setColumnCount(1);
    ui->tableWidgetServers->setColumnWidth(0, 160);
    ui->tableWidgetServers->verticalHeader()->hide();
    ui->tableWidgetServers->horizontalHeader()->hide();


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
    ui->stackedWidgetTab->setCurrentIndex(1);
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
            ui->stackedWidgetTab->setCurrentIndex(1);
        }
    }

}

void MainWindow::sendDataToServer()
{
    QString message = "Ping";
    userSocket->write(message.toUtf8());
    userSocket->flush();
}

void MainWindow::addServer()
{
    QString serverName = ui->lineEditServerName->text();
    QString serverIP = ui->lineEditServerIP->text();
    QString serverPort = ui->lineEditServerPort->text();


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

}

void MainWindow::sendAuthorizationData()
{
    QString login = ui->lineEditLogin->text();
    QString password = ui->lineEditPassword->text();

    QString authorizationData = "AUT|" + login + "|" + password;

    userSocket->write(authorizationData.toUtf8());
    userSocket->flush();
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
    sendDataToServer();
}


void MainWindow::on_pushButton_clicked()
{
    addServer();
}


void MainWindow::on_pushButtonBack_2_clicked()
{
    ui->stackedWidgetTab->setCurrentIndex(0);
}


void MainWindow::on_pushButtonAuthorization_clicked()
{
    sendAuthorizationData();
}

