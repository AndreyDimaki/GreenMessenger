#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "clientcontroller.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->sendMessageButton, &QPushButton::clicked, this, &MainWindow::sendMessage);

    ui->messageTable->setColumnCount(1);

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
    QString str = QString::fromStdString(message->content());
    QTableWidgetItem* tmp = new QTableWidgetItem(str);
    tmp->setTextAlignment(Qt::AlignRight);
    _items.append(tmp);
    ui->messageTable->insertRow(ui->messageTable->rowCount());
    ui->messageTable->setItem(ui->messageTable->rowCount()-1, 0, tmp);
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

