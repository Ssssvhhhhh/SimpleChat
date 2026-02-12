#include "userstatuswidget.h"
#include "ui_userstatuswidget.h"

UserStatusWidget::UserStatusWidget(QWidget *parent, const QString& userName, int userId, const QString& chatType, const QString& status)
    : QWidget(parent)
    , ui(new Ui::UserStatusWidget)
{
    ui->setupUi(this);
    ui->pushButton->setText(userName);
    ui->pushButtonAddInGroupChat->hide();
    userNameR = userName;
    userIdR = userId;
    type = chatType;
    statusR = status;

    if(type == "group")
    {
        ui->radioButton->hide();
    }
    if(statusR == "online")
    {
        ui->radioButton->setChecked(true);
    }
    ui->radioButton->setEnabled(false);
}

UserStatusWidget::~UserStatusWidget()
{
    delete ui;
}

void UserStatusWidget::on_pushButton_clicked()
{
    emit userNameClicked(userIdR, type);
}

void UserStatusWidget::on_pushButtonAddInGroupChat_clicked()
{
    emit userIdForGroup(userIdR);
}

void UserStatusWidget::showButton()
{
    ui->pushButtonAddInGroupChat->show();
}

void UserStatusWidget::hideButton()
{
    ui->pushButtonAddInGroupChat->hide();
}

QString UserStatusWidget::getType()
{
    return type;
}

QString UserStatusWidget::getStatus()
{
    return statusR;
}

int UserStatusWidget::getId()
{
    return userIdR;
}

void UserStatusWidget::setStatus(bool Status)
{
    if(Status)
    {
        ui->radioButton->setChecked(true);
        statusR = "online";
    }
    else
    {
        ui->radioButton->setChecked(false);
        statusR = "offline";
    }
}




