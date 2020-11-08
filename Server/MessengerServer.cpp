#include <iostream>

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
    connect(clientConnection, SIGNAL(readyRead()), this, SLOT(receiveDataFromClient()) );
    _clientConnections.push_back(clientConnection);
}

void MessengerServer::receiveDataFromClient()
{
    QTcpSocket* clientConnection = dynamic_cast<QTcpSocket*>(sender());

    if( !clientConnection ) return;
    if( clientConnection->state() == QAbstractSocket::ConnectedState ) {
        QByteArray read = clientConnection->readAll();
        QString readBuf = QString::fromLatin1(read);
        processClientMessage(readBuf);
    }
}


void MessengerServer::processClientMessage(QString& message)
{
    std::cout << message.toStdString() << "\n";
}
