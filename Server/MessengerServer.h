#ifndef MESSENGERSERVER_H
#define MESSENGERSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

static constexpr quint16 MessengerPort = 5025;

class MessengerServer : public QObject
{
    Q_OBJECT

public:
    MessengerServer(QObject* parent);
    ~MessengerServer();

private:

    ///
    /// \brief processNewConnection - Заносит новое соединение в список подключившихся клиентов
    ///
    void processNewConnection();

    ///
    /// \brief processClientMessage - Обрабатывает сообщение от клиента
    /// \param message
    ///
    void processClientMessage(QString &message);
    QTcpServer* _tcpServer;
    QList<QTcpSocket*> _clientConnections;


};

#endif // MESSENGERSERVER_H
