#ifndef CUSTOMWIDGET_H
#define CUSTOMWIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QMessageBox>

struct serverInformationData
{
    QString serverNameS;
    QString serverIPS;
    int serverPortS;

};

namespace Ui {
class CustomWidget;
}

class CustomWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CustomWidget(QWidget *parent = nullptr);
    CustomWidget(QWidget *parent, QTcpSocket* mainSocket, QString serverName, QString serverIP, int serverPort);
    void connectToCurrentServer(QTcpSocket* userSocket);
    ~CustomWidget();



private slots:
    void on_pushButtonConnect_clicked();

private:
    Ui::CustomWidget *ui;
    serverInformationData serverStruct;
    QTcpSocket* userSocketFromMain;
};

#endif // CUSTOMWIDGET_H
