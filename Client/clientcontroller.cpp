#include "clientcontroller.h"

ClientController::ClientController(QObject *parent) :
    QObject(parent),
    _user(nullptr)
{
    _socket = new QTcpSocket(this);
}

ClientController::~ClientController()
{
    _socket->deleteLater();
}

void ClientController::sendMessage(const Message* message)
{

}

User* ClientController::user()
{
    if (_user )
    {
        return _user;
    }
    return nullptr;
}
