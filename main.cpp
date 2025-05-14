#include "mainwindow.h"
#include <QApplication>
#include <QWidget>
#include <windows.h>
#include <QPalette>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

int main(int argc, char *argv[])
{
    SetConsoleOutputCP(65001);

    QApplication a(argc, argv);
    MainWindow w;

    w.show();
    return a.exec();
}
