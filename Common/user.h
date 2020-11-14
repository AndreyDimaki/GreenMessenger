#ifndef USER_H
#define USER_H

#include <QObject>

class QTcpSocket;

class User
{
public:
    explicit User(int id, const std::string& name, QTcpSocket* socket);

    int id() const
    {
        return _ID;
    }

    const std::string& name() const
    {
        return _name;
    }

    QTcpSocket* socket()
    {
        return _socket;
    }

    void setSocket(QTcpSocket* socket)
    {
        _socket = socket;
    }

private:
    int _ID;
    std::string _name;
    QTcpSocket* _socket;

};

#endif // USER_H
