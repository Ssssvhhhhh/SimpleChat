#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "customwidget.h"
#include "userstatuswidget.h"

#include <QMainWindow>
#include <QMessageBox>

#include <QTcpSocket>

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include <QStandardItemModel>
#include <QCloseEvent>
#include <QTimer>

#include <qtreewidget.h>


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
    void addServer();
    void sendAuthorizationData();
    void addUserFullNameInTab(const QString& usersFullName);
    void sendMessageToCurrentUser();
    void onUserNameRecevied(int id, QString type);
    void onChatTypeRecevide(QString type);
    void getAllMessagesInGroupChat(int chatId);
    void getAllMessagesInChat(int senderId, int reciverId);
    void loadChatMessages(const QString& chatMessages);
    void loadGroupChatMessages(const QString& groupMessages);
    void addUserInGroup(int userId);
    void sendDataToCreateGroupChat(QList<int> usersIdsForGroup);
    void changeUserStatus(int userId, QString status);
    void showAddButtonOnWidgets(bool& isShow);
    void sendRegistrationData();
    void labelError(QLabel* errorLabel);
    void showConnectionLabel();
private slots:
    void on_pushButtonOpenCloseTab_clicked();
    void on_pushButtonAddServer_clicked();
    void on_pushButtonBack_clicked();
    void on_pushButtonSend_clicked();
    void on_pushButton_clicked();
    void on_pushButtonAuthorization_clicked();
    void on_pushButtonCreateGroup_clicked();
    void on_pushButtonSendGroupData_clicked();
    void on_pushButtonBackToChats_clicked();
    void closeEvent(QCloseEvent* event); // For test delete later
    void on_pushButtonReg_clicked();
    void on_pushButtonSignUp_clicked();
    void on_pushButtonBck_clicked();

private:
    Ui::MainWindow *ui;
    bool isTabVisible = true;
    QTcpSocket* userSocket;
    CustomWidget* serverWidget;
    UserStatusWidget* userWidget;
    QTreeWidgetItem *serverItem ; // подумать о другой реализации (другие сервера)
    bool isAuthorized = false;
    bool isAddButtonShow = false;
    QMap<int, QString> usersNamesAndId;
    QMap<int, QString> groupChats;
    int userId;
    int currentChatOrUserId;
    QString currentChatType;
    QList<int> usersIdsForGroup;


};
#endif // MAINWINDOW_H
