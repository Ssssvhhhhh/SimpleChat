#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

private slots:
    void on_pushButtonOpenCloseTab_clicked();
    void on_pushButtonAddServer_clicked();
    void on_pushButtonBack_clicked();

private:
    Ui::MainWindow *ui;
    bool isTabVisible = true;
};
#endif // MAINWINDOW_H
