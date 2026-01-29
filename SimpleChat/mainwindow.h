#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "customwidget.h"

#include <QMainWindow>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void opencloseTab();
    void openServerCreationSettings();
    void openCloseServerUsers();


    void readServerResponse();
    void sendDataToServer();
    void addServer();

    void sendAuthorizationData();

private slots:
    void on_pushButtonOpenCloseTab_clicked();
    void on_pushButtonAddServer_clicked();
    void on_pushButtonBack_clicked();
    void on_pushButtonSend_clicked();
    void on_pushButton_clicked();
    void on_pushButtonBack_2_clicked();

    void on_pushButtonAuthorization_clicked();

private:
    Ui::MainWindow *ui;
    bool isTabVisible = true;
    QTcpSocket* userSocket;
    CustomWidget* serverWidget;
    bool isAuthorized = false;

};
#endif // MAINWINDOW_H
