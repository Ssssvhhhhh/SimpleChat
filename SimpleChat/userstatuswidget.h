#ifndef USERSTATUSWIDGET_H
#define USERSTATUSWIDGET_H

#include <QWidget>

namespace Ui {
class UserStatusWidget;
}

class UserStatusWidget : public QWidget
{
    Q_OBJECT

public:
    UserStatusWidget(QWidget *parent, const QString& userName, int userId, const QString& chatType, const QString& status);
    ~UserStatusWidget();
    void showButton();
    void hideButton();
    QString getType();
    QString getStatus();
    int getId();
    void setStatus(bool Status);
private slots:
    void on_pushButton_clicked();
    void on_pushButtonAddInGroupChat_clicked();

signals:
    void userNameClicked(int userId, QString chatType);
    void userIdForGroup(int userId);

private:
    Ui::UserStatusWidget *ui;
    QString userNameR;
    int userIdR;
    QString type;
    QString statusR;
};

#endif // USERSTATUSWIDGET_H
