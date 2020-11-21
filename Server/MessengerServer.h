#ifndef MESSENGERSERVER_H
#define MESSENGERSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

#include "../Common/user.h"

class Message;

class MessengerServer : public QObject
{
    Q_OBJECT

public:
    MessengerServer(QObject* parent);
    ~MessengerServer();

public slots:
    void receiveDataFromClient();

private:

    ///
    /// \brief Заносит новое соединение в список подключившихся клиентов
    ///
    void processNewConnection();

    ///
    /// \brief Обрабатывает сообщение от клиента
    /// \param message
    ///
    void processClientMessage(std::string& messageStr, QTcpSocket* clientConnection);
    QTcpServer* _tcpServer;
    std::list<QTcpSocket*> _clientConnections;

    std::list<User> _userList;
    void readUserList(const std::string& userListFileName);
    void saveUserList(const std::string& userListFileName);

    void processCreateNewUserMsg(Message* message, QTcpSocket* clientConnection);
    void processLoginMsg(Message* message, QTcpSocket* clientConnection);
    void processSendMsg(Message* message);
};

#endif // MESSENGERSERVER_H
