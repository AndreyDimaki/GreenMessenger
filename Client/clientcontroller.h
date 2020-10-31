#ifndef CLIENTCONTROLLER_H
#define CLIENTCONTROLLER_H

#include <QObject>
#include <QTcpSocket>

#include "../Common/message.h"
#include "../Common/user.h"

class ClientController : public QObject
{
    Q_OBJECT
public:
    explicit ClientController(QObject *parent = nullptr);
    ~ClientController();

    void sendMessage(const Message* message);

    User* user();

signals:
    void messageSendSuccess(const Message* message);
    void messageSendError(const Message* message);
    void userLoggedIn(const User* const user);
    void userLoginError();
    void messageReceived(const Message* message);

private:
    QTcpSocket* _socket;
    User* _user;
};

#endif // CLIENTCONTROLLER_H
