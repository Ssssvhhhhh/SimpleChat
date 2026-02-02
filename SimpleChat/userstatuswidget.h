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
    UserStatusWidget(QWidget *parent, QString userName, int userId);
    ~UserStatusWidget();

private slots:
    void on_pushButton_clicked();

signals:
    void userNameClicked(int userId);

private:
    Ui::UserStatusWidget *ui;
    QString userNameR;
    int userIdR;
};

#endif // USERSTATUSWIDGET_H
