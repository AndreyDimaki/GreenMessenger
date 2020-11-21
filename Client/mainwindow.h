#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>

#include <QTableWidgetItem>

#include "usermessages.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class ClientController;

enum class LoggingStage
{
    LS_NotLoggedIn = 1,
    LS_InProgress,
    LS_LoggedIn
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // Выбран другой пользователь в таблице
    void onReceivingUserChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);

    // Сигналы из контроллера
    void onMessageSendSuccess(const Message* message);
    void onMessageSendError(const Message* message);
    void onMessageReceived(const Message* message);

signals:
    void trySendMessage(const Message* message);

private:
    void sendMessage();
    void tryCreateUser();
    void tryLogin();
    void logout();
    void appendSentMessage(const Message* message);
    void appendReceivedMessage(const Message* message);
    void refreshMessagesList(UserMessages &user);
    void appendMessageToTable(const Message *message, bool isReceived);

    Ui::MainWindow *ui;

    LoggingStage _loggingStage = LoggingStage::LS_NotLoggedIn;

    int _currentSenderID = -1;
    ClientController* _controller;
    QThread* _thread;
    QList<UserMessages> _userList;
    UserMessages* _currentReceivingUser = nullptr;
    QList<Message*> _outputMessageBuffer;
    QList<QTableWidgetItem*> _items;
};

#endif // MAINWINDOW_H

