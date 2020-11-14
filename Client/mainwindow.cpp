#include <iostream>

#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "clientcontroller.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->sendMessageButton, &QPushButton::clicked, this, &MainWindow::sendMessage);
    connect(ui->createButton, &QPushButton::clicked, this, &MainWindow::tryCreateUser);
    connect(ui->loginButton, &QPushButton::clicked, this, &MainWindow::tryLogin);
    connect(ui->actionLogout, &QAction::triggered, this,  &MainWindow::logout);
    connect(ui->actionExit, &QAction::triggered, this, []()
    {
        QApplication::exit();
    });

    ui->messageTable->setColumnCount(1);
    ui->stackedWidget->setCurrentIndex(0);

    _controller = new ClientController();
    _thread = new QThread(this);
    _controller->moveToThread(_thread);
    _thread->start();

    connect(this, &MainWindow::trySendMessage, _controller, &ClientController::sendMessage);
    connect(_controller, &ClientController::messageSendSuccess,
            this, &MainWindow::onMessageSendSuccess);
    connect(_controller, &ClientController::messageSendError,
            this, &MainWindow::onMessageSendError);
    connect(_controller, &ClientController::messageReceived,
            this, &MainWindow::onMessageReceived);

    _controller->connectToHost();
}

MainWindow::~MainWindow()
{
    _thread->quit();
    _thread->wait();

    _controller->deleteLater();
    delete ui;
}

void MainWindow::sendMessage()
{
    if (ui->newMessageText->text().isEmpty())
        return;
    auto message = new Message(_currentSenderID,
                               _currentReceiverID,
                               MessageTypeID::MSG_SendMessage,
                               ui->newMessageText->text().toStdString());
    _messageBuffer.append(message);
    emit trySendMessage(message);
}

void MainWindow::tryCreateUser()
{
    if (!_controller->isSocketConnected())
        return;
    auto message = new Message(0, 0, MessageTypeID::MSG_CreateNewUser,
                               ui->newUserNameEdit->text().toStdString());
    emit trySendMessage(message);
}

void MainWindow::tryLogin()
{
    if (!_controller->isSocketConnected())
        return;
    auto message = new Message(0, 0, MessageTypeID::MSG_LogIn,
                               ui->userNameEdit->text().toStdString());
    _loggingStage = LoggingStage::LS_InProgress;
    emit trySendMessage(message);
}

void MainWindow::logout()
{
    _loggingStage = LoggingStage::LS_NotLoggedIn;
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::onMessageSendSuccess(const Message* message)
{
    int messageIndex = _messageBuffer.lastIndexOf(const_cast<Message*>(message));
    if (messageIndex > -1)
    {
        appendSentMessage(message);
        _messageBuffer.removeAt(messageIndex);
        delete message;
    }
}

void MainWindow::onMessageSendError(const Message* message)
{
    int messageIndex = _messageBuffer.lastIndexOf(const_cast<Message*>(message));
    if (messageIndex > -1)
    {
        _messageBuffer.removeAt(messageIndex);
        delete message;
    }
}

void MainWindow::onMessageReceived(const Message* message)
{
    std::cout << message->toOutputString() << std::endl;
    if (_loggingStage == LoggingStage::LS_NotLoggedIn)
        return;

    if (_loggingStage == LoggingStage::LS_InProgress)
    {
        if (message->id() == MessageTypeID::MSG_LoginSuccess)
        {
            _loggingStage = LoggingStage::LS_LoggedIn;
            setWindowTitle("GreenMessenger - "+QString::fromStdString(message->content()));
            ui->stackedWidget->setCurrentIndex(1);
        }
        return;
    }

    if (_loggingStage == LoggingStage::LS_LoggedIn)
    {
        QString str = QString::fromStdString(message->content());
        QTableWidgetItem* tmp = new QTableWidgetItem(str);
        tmp->setTextAlignment(Qt::AlignRight);
        _items.append(tmp);
        ui->messageTable->insertRow(ui->messageTable->rowCount());
        ui->messageTable->setItem(ui->messageTable->rowCount()-1, 0, tmp);
    }
}

void MainWindow::appendSentMessage(const Message* message)
{
    QString str = QString::fromStdString(message->content());
    QTableWidgetItem* tmp = new QTableWidgetItem(str);
    tmp->setTextAlignment(Qt::AlignLeft);
    _items.append(tmp);
    ui->messageTable->insertRow(ui->messageTable->rowCount());
    ui->messageTable->setItem(ui->messageTable->rowCount()-1, 0, tmp);
}

