#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "boardwindow.h"
// #include <iostream>
#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QPalette>
#include <QTextStream>
using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);  // 先初始化UI

    // 创建QPixmap并加载图片
    QPixmap bkgnd(":/res/gobang_SZL.png");

    // 缩放图片以适应窗口大小
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);

    // 设置调色板
    QPalette palette;
    palette.setBrush(QPalette::Window, QBrush(bkgnd));
    this->setPalette(palette);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_GameBegin_clicked()
{
    // 开始对局 传递棋盘大小和模式 创建对局
    QString boardQString = ui->comboBox_board->currentText();
    QString modelQString = ui->comboBox_model->currentText();

    // cout << boardQString.toUtf8().constData() << endl;
    // cout << modelQString.toStdString() << endl;

    QTextStream(stdout) << boardQString << Qt::endl;
    QTextStream(stdout) << modelQString << Qt::endl;

    // 使用 new 创建窗口，防止析构
    boardWindow *board = new boardWindow(boardQString, modelQString);

    // 设置关闭时自动销毁，防止内存泄漏
    board->setAttribute(Qt::WA_DeleteOnClose);

    // 显示窗口
    board->show();
}

void MainWindow::on_comboBox_model_currentTextChanged(const QString &arg1)
{
    QTextStream(stdout) << "change to" << arg1 << Qt::endl;
}
