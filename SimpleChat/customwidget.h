#ifndef CUSTOMWIDGET_H
#define CUSTOMWIDGET_H

#include <QWidget>
#include <QSslSocket>
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
    CustomWidget(QWidget *parent, QSslSocket* mainSocket, QString serverName, QString serverIP, int serverPort);
    ~CustomWidget();
    void connectToCurrentServer(QSslSocket* userSocket);


private slots:
    void on_pushButtonConnect_clicked();

private:
    Ui::CustomWidget *ui;
    serverInformationData serverStruct;
    QSslSocket* userSocketFromMain;
};

#endif // CUSTOMWIDGET_H
