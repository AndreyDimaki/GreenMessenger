#ifndef MESSENGERSERVER_H
#define MESSENGERSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

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
    void processClientMessage(std::string &messageStr);
    QTcpServer* _tcpServer;
    QList<QTcpSocket*> _clientConnections;


};

#endif // MESSENGERSERVER_H
