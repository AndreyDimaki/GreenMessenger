#include "clientcontroller.h"

ClientController::ClientController(QObject *parent) : QObject(parent)
{
    _socket = new QTcpSocket(this);
}

ClientController::~ClientController()
{
    _socket->deleteLater();
}
