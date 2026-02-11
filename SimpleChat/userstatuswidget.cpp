#include "userstatuswidget.h"
#include "ui_userstatuswidget.h"

UserStatusWidget::UserStatusWidget(QWidget *parent, QString userName, int userId, QString chatType)
    : QWidget(parent)
    , ui(new Ui::UserStatusWidget)
{
    ui->setupUi(this);
    ui->pushButton->setText(userName);
    ui->pushButtonAddInGroupChat->hide();
    userNameR = userName;
    userIdR = userId;
    type = chatType;

    if(type == "group")
    {
        ui->radioButton->hide();
        //ui->pushButtonAddInGroupChat->;
    }
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




