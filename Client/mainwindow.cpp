#include <QMessageBox>
#include <iostream>
#include <sstream>

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

    connect(ui->userTableWidget, &QTableWidget::currentCellChanged,
            this, &MainWindow::onReceivingUserChanged);

    ui->stackedWidget->setCurrentIndex(0);

    ui->messageTable->setColumnCount(1);
    ui->messageTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

    ui->userTableWidget->setColumnCount(1);
    ui->userTableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->userTableWidget->horizontalHeader()->setHighlightSections(false);
    ui->userTableWidget->horizontalHeader()->setSectionsClickable(false);
    ui->userTableWidget->horizontalHeader()->setSectionsMovable(false);
    ui->userTableWidget->setHorizontalHeaderLabels(QStringList() << "Contacts");

    _controller = new ClientController();
    _thread = new QThread(this);
    _controller->moveToThread(_thread);
    _thread->start();

    // Применяем Qt::UniqueConnection, поскольку в данной модели владения
    // сообщениями не желательно иметь более одного соединения с сигналами,
    // в которых передаются сообщения
    connect(this, &MainWindow::trySendMessage,
            _controller, &ClientController::sendMessage, Qt::UniqueConnection);
    connect(_controller, &ClientController::messageSendSuccess,
            this, &MainWindow::onMessageSendSuccess, Qt::UniqueConnection);
    connect(_controller, &ClientController::messageSendError,
            this, &MainWindow::onMessageSendError, Qt::UniqueConnection);
    connect(_controller, &ClientController::messageReceived,
            this, &MainWindow::onMessageReceived, Qt::UniqueConnection);

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
    int currentReceiverID = _currentReceivingUser ? _currentReceivingUser->id() : 0;
    auto message = new Message(_currentSenderID,
                               currentReceiverID,
                               MessageTypeID::MSG_SendMessage,
                               ui->newMessageText->text().toStdString());
    _outputMessageBuffer.append(message);
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
    int messageIndex = _outputMessageBuffer.lastIndexOf(const_cast<Message*>(message));
    if (messageIndex > -1)
    {
        appendSentMessage(message);
        _outputMessageBuffer.removeAt(messageIndex);
        delete message;
    }
}

void MainWindow::onMessageSendError(const Message* message)
{
    int messageIndex = _outputMessageBuffer.lastIndexOf(const_cast<Message*>(message));
    if (messageIndex > -1)
    {
        _outputMessageBuffer.removeAt(messageIndex);
        delete message;
    }
}

void MainWindow::onMessageReceived(const Message* message)
{
    std::cout << message->toOutputString() << std::endl;
    switch(_loggingStage)
    {
        case LoggingStage::LS_NotLoggedIn:
            if (message->id() == MessageTypeID::MSG_CannotCreateUser)
            {
                QMessageBox::critical(this, "Error",
                                      "Cannot create user : "+QString::fromStdString(message->content()));
            }
        break;
        case LoggingStage::LS_InProgress:
            if (message->id() == MessageTypeID::MSG_LoginSuccess)
            {
                _userList.clear();
                ui->userTableWidget->clearContents();
                ui->userTableWidget->setRowCount(0);
                _loggingStage = LoggingStage::LS_LoggedIn;
                setWindowTitle("GreenMessenger - "+QString::fromStdString(message->content()));
                // Получили с сервера наш ID
                _currentSenderID = message->receiverID();
                qDebug() << "Current user ID : " << _currentSenderID;
                ui->stackedWidget->setCurrentIndex(1);
            }
            else
            {
                QMessageBox::critical(this, "Error",
                                      "User not found : "+QString::fromStdString(message->content()));
            }
        break;
        case LoggingStage::LS_LoggedIn:
            if (message->id() == MessageTypeID::MSG_SendMessage)
            {
                // Входящее сообщение
                appendReceivedMessage(message);
            }
            else if (message->id() == MessageTypeID::MSG_ReceiveUserList)
            {
                // Пользователь из списка
                std::vector<std::string> result;
                std::stringstream sstream(message->content());
                for (std::string subline; std::getline(sstream, subline, ','); )
                {
                    result.push_back(subline);
                }
                if (result.size() == 2)
                {
                    _userList.push_back(UserMessages(std::stoi(result[0]), result[1]));
                    QString str = QString::fromStdString(result[1]);
                    QTableWidgetItem* tmp = new QTableWidgetItem(str);
                    tmp->setFlags(tmp->flags() ^ Qt::ItemIsEditable);
                    tmp->setTextAlignment(Qt::AlignLeft);
                    ui->userTableWidget->insertRow(ui->userTableWidget->rowCount());
                    ui->userTableWidget->setItem(ui->userTableWidget->rowCount()-1, 0, tmp);
                }
                if (ui->userTableWidget->rowCount()>0)
                {
                    ui->userTableWidget->setCurrentCell(0,0);
                }
            }
        break;
    }
    delete message;
}

void MainWindow::onReceivingUserChanged(int currentRow, int currentColumn,
                                        int previousRow, int previousColumn)
{
    Q_UNUSED(currentColumn)
    Q_UNUSED(previousRow)
    Q_UNUSED(previousColumn)
    _currentReceivingUser = &_userList[currentRow];
    refreshMessagesList(*_currentReceivingUser);
}

void MainWindow::refreshMessagesList(UserMessages& user)
{
    ui->messageTable->clearContents();
    ui->messageTable->setRowCount(0);
    for (auto msg : user.messages())
    {
        bool isReceived = (msg.senderID() == user.id());
        appendMessageToTable(&msg, isReceived);
    }
}

void MainWindow::appendSentMessage(const Message* message)
{
    if (_loggingStage != LoggingStage::LS_LoggedIn)
        return;
    _currentReceivingUser->appendMessage(message);
    appendMessageToTable(message, false);
}

void MainWindow::appendReceivedMessage(const Message* message)
{
    if (_loggingStage != LoggingStage::LS_LoggedIn)
        return;

    // Если пришло сообщение от выбранного в данный момент пользователя
    if (_currentReceivingUser->id() == message->senderID())
    {
        _currentReceivingUser->appendMessage(message);
        appendMessageToTable(message, true);
    }
    else
    {
        for (auto& user : _userList)
        {
            if (user.id() == message->senderID())
            {
                user.appendMessage(message);
            }
        }
    }
}

void MainWindow::appendMessageToTable(const Message* message, bool isReceived)
{
    QString str = QString::fromStdString(message->content());
    QTableWidgetItem* tmp = new QTableWidgetItem(str);
    tmp->setFlags(tmp->flags() ^ Qt::ItemIsEditable);
    tmp->setTextAlignment(isReceived ? Qt::AlignLeft : Qt::AlignRight);
    tmp->setBackground(QBrush(isReceived ? QColor(255, 255, 224) : QColor(144, 238, 144)));
    _items.append(tmp);
    ui->messageTable->insertRow(ui->messageTable->rowCount());
    ui->messageTable->setItem(ui->messageTable->rowCount()-1, 0, tmp);
}
