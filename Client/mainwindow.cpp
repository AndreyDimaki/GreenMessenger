#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "clientcontroller.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->sendMessageButton, &QPushButton::clicked, this, &MainWindow::sendMessage);

    _controller = new ClientController();
    _thread = new QThread(this);
    _controller->moveToThread(_thread);

    connect(this, &MainWindow::trySendMessage, _controller, &ClientController::sendMessage);
    connect(_controller, &ClientController::messageSendSuccess,
            this, &MainWindow::onMessageSendSuccess);
    connect(_controller, &ClientController::messageSendError,
            this, &MainWindow::onMessageSendError);
    connect(_controller, &ClientController::messageReceived,
            this, &MainWindow::onMessageReceived);


}

MainWindow::~MainWindow()
{
    _controller->deleteLater();
    _thread->deleteLater();

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
    emit trySendMessage(message);
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

}

void MainWindow::appendSentMessage(const Message* message)
{

}

