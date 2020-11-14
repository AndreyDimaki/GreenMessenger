#include <QTimer>
#include <QEventLoop>

#include "clientcontroller.h"

ClientController::ClientController(QObject *parent) :
    QObject(parent),
    _user(nullptr)
{
    qRegisterMetaType<QAbstractSocket::SocketState>();
    qRegisterMetaType<QAbstractSocket::SocketError>();

    _socket = new QTcpSocket(this);
    connect(_socket, &QTcpSocket::readyRead,
            this, &ClientController::readData );
    connect(_socket, &QTcpSocket::stateChanged,
            this, &ClientController::stateChanged );

    connect(_socket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)),
            this, SLOT(interceptError(QAbstractSocket::SocketError)));

}

ClientController::~ClientController()
{
    _socket->deleteLater();
}

User* ClientController::user()
{
    if (_user )
    {
        return _user;
    }
    return nullptr;
}


void ClientController::sendMessage(const Message* message)
{
    if (isSocketConnected())
    {
        QString msg = QString::fromStdString(message->toOutputString() );
        QString answer = sendStringMessages( QList<QString>() << msg, 0 );
        emit messageSendSuccess(message);
    }
    else
    {
        emit messageSendError(message);
    }
}


QString ClientController::sendStringMessages(const QList<QString>& msgs, int timeOutMSec)
{
    _run = true;

    _stringMessages = msgs;

    QEventLoop wait;
    connect( this, SIGNAL( pipelineFinishing() ), &wait, SLOT( quit() ) );

    QTimer timeOutTimer;
    if( timeOutMSec > 0 ) {
        timeOutTimer.setSingleShot( true );
        timeOutTimer.setInterval( timeOutMSec );
        connect( &timeOutTimer, SIGNAL( timeout() ), &wait, SLOT( quit() ) );
        timeOutTimer.start();
    }

    if( nextPipeline() )
        wait.exec();

    _run = false;
    if( timeOutMSec > 0 ) {
        // Проверяем, выполнился ли тайм-аут
        if( !timeOutTimer.isActive() ) {
           _readBuf.clear();
           return "TIMEOUT";
        } else {
           // Если таймаут не наступил, останавливаем таймер
           timeOutTimer.stop();
        }
    }
    return _readBuf;
}

void ClientController::checkSocketConnect()
{
    if( !_run )
    {
        bool status = isSocketConnected();
        if( _oldStatus != status ) emit updateStatus();
    }
}

void ClientController::connectToHost()
{
    if( _socket->isValid() ) {
        _socket->abort();
    }
    _socket->connectToHost( "localhost", MessengerPort );
}

void ClientController::run(const QString& cmd)
{
    if( _socket->state() == QAbstractSocket::ConnectedState )
    {
        _readBuf.clear();
        _socket->write( cmd.toLatin1() );
    }
    else
    {
        _readBuf.clear();
        interceptError(_socket->error());
    }
}


void ClientController::readData()
{
    QByteArray read = _socket->readAll();

    _readBuf += QString::fromLatin1(read);

    qDebug() << _readBuf;

    /// TODO: Split readbuf;

    if(*(read.end() - 1) == '\n')
    {
        auto message = Message::createNew(_readBuf.toStdString());
        if (message)
        {
            emit messageReceived(message);
        }
        nextPipeline(); // продолжить обработку
    }
}



void ClientController::interceptError(QAbstractSocket::SocketError socketError)
{
    qInfo() << "ERROR: " << socketError;
    emit error(QString("%1").arg(socketError));
}

void ClientController::stateChanged(QAbstractSocket::SocketState state)
{
    qDebug() << "ClientController::stateChanged";
    switch( state )
    {
        case QAbstractSocket::UnconnectedState:
        {
            qInfo() << "UnconnectedState";
            QTimer::singleShot( 2000, this, &ClientController::connectToHost);
        } break;
        case QAbstractSocket::HostLookupState:
            qInfo() << "HostLookupState"; break;
        case QAbstractSocket::ConnectingState:
            qInfo() << "ConnectingState"; break;
        case QAbstractSocket::ConnectedState:
            qInfo() << "ConnectedState"; break;
        case QAbstractSocket::BoundState:
            qInfo() << "BoundState"; break;
        case QAbstractSocket::ListeningState:
            qInfo() << "ListeningState"; break;
        case QAbstractSocket::ClosingState:
            qInfo() << "ClosingState";  break;
    }
    emit updateStatus();
}

bool ClientController::nextPipeline()
{
    forever {
        if( _stringMessages.isEmpty() )
        {
            emit pipelineFinishing();
            return false;
        }

        QString msg = _stringMessages.takeFirst();
        run( msg );
    }
}

bool ClientController::isSocketConnected()
{
    if( (_socket != nullptr) && (_socket->state() == QAbstractSocket::ConnectedState) )
    {
        return true;
    }
    return false;
}
