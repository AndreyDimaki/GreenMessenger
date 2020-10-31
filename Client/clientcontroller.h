#ifndef CLIENTCONTROLLER_H
#define CLIENTCONTROLLER_H

#include <QObject>
#include <QTcpSocket>

class ClientController : public QObject
{
    Q_OBJECT
public:
    explicit ClientController(QObject *parent = nullptr);
    ~ClientController();

signals:


private:
    QTcpSocket* _socket;
};

#endif // CLIENTCONTROLLER_H
