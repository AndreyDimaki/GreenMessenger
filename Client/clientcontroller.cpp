#include <QEventLoop>

#include "clientcontroller.h"

ClientController::ClientController(QObject *parent) :
    QObject(parent)
{
    _timerTryConnection = new QTimer( this );
    _timerTryConnection->setSingleShot( false );
    _timerTryConnection->setInterval( 3000 );

    qRegisterMetaType<QAbstractSocket::SocketState>();
    qRegisterMetaType<QAbstractSocket::SocketError>();

    _socket = new QTcpSocket(this);
    connect(_socket, &QTcpSocket::readyRead,
            this, &ClientController::readData );
    connect(_socket, &QTcpSocket::stateChanged,
            this, &ClientController::stateChanged );

    connect(_socket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)),
            this, SLOT(interceptError(QAbstractSocket::SocketError)));

    QObject::connect( _timerTryConnection, &QTimer::timeout, this, &ClientController::connectToHost );
    _timerTryConnection->start();
}

ClientController::~ClientController()
{
}

void ClientController::sendMessage(const Message* message)
{
    if (isSocketConnected())
    {
        std::string msg = message->toOutputString();
        QString answer = sendStringMessage( msg );
        emit messageSendSuccess(message);
    }
    else
    {
        emit messageSendError(message);
    }
}


QString ClientController::sendStringMessage(const std::string& msg, int timeOutMSec)
{
    _run = true;
    _stringMessages << msg;

    QEventLoop wait;
    connect( this, SIGNAL( pipelineFinished() ), &wait, SLOT( quit() ) );

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
        if( _oldStatus != status ) emit statusUpdated(_connectStatus);
    }
}

void ClientController::connectToHost()
{
    if (isSocketConnected()) return;

    if ( _socket->isValid() )
    {
        _socket->abort();
    }
    _socket->connectToHost( "localhost", MessengerPort );
}

void ClientController::run(const std::string& msg)
{
    if( _socket->state() == QAbstractSocket::ConnectedState )
    {
        _readBuf.clear();
        _socket->write( msg.data() );
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

    _readBuf += QString::fromUtf8(read);

    // qDebug() << "ClientController::readData() : " << QString::fromUtf8(read);

    QStringList rcvList = _readBuf.split("\n");
    for (auto inputStr : rcvList)
    {
        // Вернем на место разделитель сообщений!
        auto message = Message::createNew((inputStr+"\n").toStdString());
        if (message)
        {
            emit messageReceived(message);
        }
    }

    // Грубо, но пока так
    _readBuf.clear();

    if(*(read.end() - 1) == '\n')
    {
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
    switch( state )
    {
        case QAbstractSocket::UnconnectedState:
        {
            _connectStatus = "UnconnectedState";
            QTimer::singleShot( 2000, this, &ClientController::connectToHost);
        } break;
        case QAbstractSocket::HostLookupState:
            _connectStatus = "HostLookupState"; break;
        case QAbstractSocket::ConnectingState:
            _connectStatus = "ConnectingState"; break;
        case QAbstractSocket::ConnectedState:
            _connectStatus = "ConnectedState"; break;
        case QAbstractSocket::BoundState:
            _connectStatus = "BoundState"; break;
        case QAbstractSocket::ListeningState:
            _connectStatus = "ListeningState"; break;
        case QAbstractSocket::ClosingState:
            _connectStatus = "ClosingState";  break;
    }
    // qDebug() << "ClientController::stateChanged : " << _connectStatus;
    emit statusUpdated(_connectStatus);
}

bool ClientController::nextPipeline()
{
    forever {
        if( _stringMessages.isEmpty() )
        {
            emit pipelineFinished();
            return false;
        }
        run( _stringMessages.takeFirst() );
    }
}

bool ClientController::isSocketConnected()
{
    return (_socket != nullptr) && (_socket->state() == QAbstractSocket::ConnectedState) ? true : false;
}
