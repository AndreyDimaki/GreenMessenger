#ifndef CLIENTCONTROLLER_H
#define CLIENTCONTROLLER_H

#include <QObject>
#include <QTcpSocket>

#include "../Common/message.h"
#include "../Common/user.h"

class ClientController : public QObject
{
    Q_OBJECT
public:
    explicit ClientController(QObject *parent = nullptr);
    ~ClientController();

    void sendMessage(const Message* message);

    User* user();

    void connectToHost();

public slots:
    void interceptError(QAbstractSocket::SocketError socketError);

signals:
    void error( const QString &err );
    void pipelineFinishing();
    void updateStatus();


    void messageSendSuccess(const Message* message);
    void messageSendError(const Message* message);
    void userLoggedIn(const User* const user);
    void userLoginError();
    void messageReceived(const Message* message);

private:
    QTcpSocket* _socket;
    User* _user;

    QString        _readBuf;
    QList<QString> _stringMessages; // очередь строковых сообщений
    bool _run;
    bool _oldStatus = false;

    void readData();
    bool nextPipeline();
    void stateChanged(QAbstractSocket::SocketState state);
    bool isSocketConnected();
    void run(const QString &cmd);
    QString sendStringMessages(const QList<QString> &msgs, int timeOutMSec);
    void checkSocketConnect();
};

// Q_DECLARE_METATYPE(QAbstractSocket::SocketState)

#endif // CLIENTCONTROLLER_H
