#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::opencloseTab()
{
    if(isTabVisible)
    {
        ui->frameTab->hide();
        isTabVisible = false;
    }
    else
    {
        ui->frameTab->show();
        isTabVisible = true;
    }
}

void MainWindow::openServerCreationSettings()
{
    ui->stackedWidget->setCurrentIndex(1);

}

void MainWindow::on_pushButtonOpenCloseTab_clicked()
{
    opencloseTab();
}


void MainWindow::on_pushButtonAddServer_clicked()
{
    openServerCreationSettings();
}


void MainWindow::on_pushButtonBack_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

