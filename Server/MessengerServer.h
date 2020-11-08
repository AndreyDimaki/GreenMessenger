#ifndef MESSENGERSERVER_H
#define MESSENGERSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

#include "../Common/message.h"

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
    void processClientMessage(QString &message);
    QTcpServer* _tcpServer;
    QList<QTcpSocket*> _clientConnections;


};

#endif // MESSENGERSERVER_H
