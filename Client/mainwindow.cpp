#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "clientcontroller.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _controller = new ClientController();
    _thread = new QThread(this);
    _controller->moveToThread(_thread);
}

MainWindow::~MainWindow()
{
    _controller->deleteLater();
    _thread->deleteLater();

    delete ui;
}

