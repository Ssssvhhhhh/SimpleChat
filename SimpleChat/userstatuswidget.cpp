#include "userstatuswidget.h"
#include "ui_userstatuswidget.h"

UserStatusWidget::UserStatusWidget(QWidget *parent, QString userName, int userId)
    : QWidget(parent)
    , ui(new Ui::UserStatusWidget)
{
    ui->setupUi(this);
    ui->pushButton->setText(userName);
    userNameR = userName;
    userIdR = userId;
}

UserStatusWidget::~UserStatusWidget()
{
    delete ui;
}

void UserStatusWidget::on_pushButton_clicked()
{
    emit userNameClicked(userIdR);
}

