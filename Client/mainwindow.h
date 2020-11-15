#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>

#include <QTableWidgetItem>

#include "../Common/message.h"
#include "../Common/user.h"

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

    void onMessageSendSuccess(const Message* message);
    void onMessageSendError(const Message* message);
    void onMessageReceived(const Message* message);

    void onReceivingUserChanged(int rcvUserIndex);

signals:
    void trySendMessage(const Message* message);

private:
    void sendMessage();
    void tryCreateUser();
    void tryLogin();
    void logout();
    void appendSentMessage(const Message *message);

    Ui::MainWindow *ui;

    LoggingStage _loggingStage = LoggingStage::LS_NotLoggedIn;

    int _currentSenderID = -1;
    int _currentReceiverID = -1;
    ClientController* _controller;
    QThread* _thread;
    QList<User> _userList;
    QList<Message*> _messageBuffer;
    QList<QTableWidgetItem*> _items;

};
#endif // MAINWINDOW_H

