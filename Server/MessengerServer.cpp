#include "MessengerServer.h"

MessengerServer::MessengerServer(QObject* parent) :
    QObject(parent)
{
    _tcpServer = new QTcpServer(this);
    connect(_tcpServer, &QTcpServer::newConnection,
            this, &MessengerServer::processNewConnection);
    _tcpServer->listen(QHostAddress::Any, MessengerPort);
}

MessengerServer::~MessengerServer()
{
    _tcpServer->deleteLater();
}

void MessengerServer::processNewConnection()
{
    QTcpSocket* clientConnection = _tcpServer->nextPendingConnection();
    connect(clientConnection, &QAbstractSocket::disconnected,
            clientConnection, &QObject::deleteLater);
    _clientConnections.push_back(clientConnection);
}

void MessengerServer::processClientMessage(QString& message)
{

}
