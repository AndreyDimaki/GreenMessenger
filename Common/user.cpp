#include "user.h"

User::User(int id, const std::string& name, QTcpSocket* socket) :
    _ID(id),
    _name(name),
    _socket(socket)
{

}
