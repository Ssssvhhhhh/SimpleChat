#include "userstatuswidget.h"
#include "ui_userstatuswidget.h"

UserStatusWidget::UserStatusWidget(QWidget *parent, QString userName)
    : QWidget(parent)
    , ui(new Ui::UserStatusWidget)
{
    ui->setupUi(this);
    ui->pushButton->setText(userName);
}

UserStatusWidget::~UserStatusWidget()
{
    delete ui;
}
