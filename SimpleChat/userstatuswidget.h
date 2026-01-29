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
    UserStatusWidget(QWidget *parent, QString userName);
    ~UserStatusWidget();

private:
    Ui::UserStatusWidget *ui;
};

#endif // USERSTATUSWIDGET_H
