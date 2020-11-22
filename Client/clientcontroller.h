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
    void connectToHost();
    bool isSocketConnected();

public slots:
    void interceptError(QAbstractSocket::SocketError socketError);

signals:
    void error( const QString &err );
    void pipelineFinished();
    void statusUpdated(const QString& status);

    void messageSendSuccess(const Message* message);
    void messageSendError(const Message* message);
    void userLoggedIn(const User* const user);
    void userLoginError();
    void messageReceived(const Message* message);

private:
    QTcpSocket* _socket;
    QString _readBuf;
    QList<std::string> _stringMessages; // очередь строковых сообщений
    bool _run;
    bool _oldStatus = false;
    QString _connectStatus;
    QTimer* _timerTryConnection;

    void readData();
    bool nextPipeline();
    void stateChanged(QAbstractSocket::SocketState state);
    void run(const std::string &msg);
    QString sendStringMessage(const std::string& msg, int timeOutMSec = 0);
    void checkSocketConnect();
};

// Q_DECLARE_METATYPE(QAbstractSocket::SocketState)

#endif // CLIENTCONTROLLER_H
