#include <iostream>
#include <fstream>
#include <sstream>

#include "MessengerServer.h"
#include "../Common/message.h"

static const std::string UserListFileName = "userList.csv";

MessengerServer::MessengerServer(QObject* parent) :
    QObject(parent)
{
    readUserList(UserListFileName);

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
    QTcpSocket* clientConnection = qobject_cast<QTcpSocket*>(sender());

    if( !clientConnection ) return;
    if( clientConnection->state() == QAbstractSocket::ConnectedState ) {
        std::string readBuf = clientConnection->readAll().toStdString();
        processClientMessage(readBuf, clientConnection);
    }
}


void MessengerServer::processClientMessage(std::string& messageStr, QTcpSocket* clientConnection)
{    
    std::cout << "MSG : " << messageStr;
    auto parsedMessage = Message::createNew(messageStr);
    if (parsedMessage)
    {
        /// Здесь обрабатываем принятое сообщение
        switch (parsedMessage->id())
        {
        case MessageTypeID::MSG_CreateNewUser:
            processCreateNewUserMsg(parsedMessage, clientConnection);
        break;
        case MessageTypeID::MSG_LogIn:
            processLoginMsg(parsedMessage, clientConnection);
        break;
        case MessageTypeID::MSG_SendMessage:
            processSendMsg(parsedMessage);
        break;
        default:
            /// Остальные типы сообщений здесь не обрабатываем
        break;
        }
        delete parsedMessage;
    }
    else
    {
        std::cout << "Cannot parse message " << messageStr << "\n";
    }
}

void MessengerServer::readUserList(const std::string& userListFileName)
{
    std::ifstream ifs;
    ifs.open(userListFileName, std::ifstream::in);
    if (!ifs.is_open())
        return;

    _userList.clear();
    for (std::string line; std::getline(ifs, line); )
    {
        std::vector<std::string> result;
        std::stringstream sstream(line);
        for (std::string subline; std::getline(sstream, subline, ','); )
        {
            result.push_back(subline);
        }
        if (result.size() == 2)
        {
            _userList.emplace_back(std::stoi(result[0]), result[1], nullptr);
        }
    }

    std::cout << "Registered users:" << std::endl;
    for (auto user : _userList)
    {
        std::cout << user.id() << "," << user.name() << std::endl;
    }

}

void MessengerServer::saveUserList(const std::string& userListFileName)
{
    std::ofstream ofs;
    ofs.open(userListFileName, std::ofstream::out);
    if (!ofs.is_open())
        return;
    for (auto user : _userList)
    {
        ofs << user.id() << "," << user.name() << std::endl;
    }
}

void MessengerServer::processCreateNewUserMsg(Message* message,  QTcpSocket* clientConnection)
{
    // Проверяем, есть ли уже такой пользователь
    auto it = std::find_if(_userList.begin(), _userList.end(),
                           [message](const User& user){ return user.name() == message->content(); } );

    // Если такого пользователя нет, добавляем его в список
    // и отправляем соообщение, что пользователь добавлен
    if (it == _userList.end())
    {
        int newUserID = 0;
        for(auto& user : _userList)
        {
            if (user.id() > newUserID)
            {
                newUserID = user.id();
            }
        }
        newUserID++;
        _userList.emplace_back(newUserID, message->content(), clientConnection);
        saveUserList(UserListFileName);
        Message msg(0,newUserID, MessageTypeID::MSG_UserCreated, message->content());
        std::string messageStr = msg.toOutputString();
        clientConnection->write(messageStr.data(), messageStr.length());
    }
    else
    {
        Message msg(0,0, MessageTypeID::MSG_CannotCreateUser, message->content());
        std::string messageStr = msg.toOutputString();
        clientConnection->write(messageStr.data(), messageStr.length());
    }
}

void MessengerServer::processLoginMsg(Message* message, QTcpSocket* clientConnection)
{
    // Проверяем, есть ли уже такой пользователь
    auto it = std::find_if(_userList.begin(), _userList.end(),
                           [message](const User& user){ return user.name() == message->content(); } );

    // Если пользователь есть, отправляем соообщение, что вход совершен
    if (it != _userList.end())
    {
        Message msg(0,it->id(), MessageTypeID::MSG_LoginSuccess, message->content());
        it->setSocket(clientConnection);
        std::string messageStr = msg.toOutputString();
        clientConnection->write(messageStr.data(), messageStr.length());
        for (auto& user : _userList)
        {
            // Отправляем список пользователей, имеющихся в системе
            // Здесь можно отправлять список только активных пользователей
            if (user.id() != it->id())
            {
                std::stringstream ss;
                ss << user.id() << "," << user.name();
                Message msg(0,it->id(), MessageTypeID::MSG_ReceiveUserList, ss.str());
                std::string messageStr = msg.toOutputString();
                clientConnection->write(messageStr.data(), messageStr.length());
            }
        }
    }
    else
    {
        // Нет такого пользователя!
        Message msg(0,0, MessageTypeID::MSG_CannotLogin, message->content());
        std::string messageStr = msg.toOutputString();
        clientConnection->write(messageStr.data(), messageStr.length());
    }
}

void MessengerServer::processSendMsg(Message* message)
{
    // Проверяем, есть ли получатель сообщения
    auto receiver = std::find_if(_userList.begin(), _userList.end(),
                           [message](const User& user){ return user.id() == message->receiverID(); } );

    // На всякий случай проверяем, есть ли отправитель сообщения в нашем списке пользователей
    auto sender = std::find_if(_userList.begin(), _userList.end(),
                           [message](const User& user){ return user.id() == message->senderID(); } );

    // Финальная проверка и отправка сообщения получателю
    if ((receiver != _userList.end()) &&
        (sender != _userList.end()) &&
        (receiver->socket() != nullptr))
    {
        std::string messageStr = message->toOutputString();
        std::cout << "processSendMsg : " << messageStr;
        receiver->socket()->write(messageStr.data(), messageStr.length());
    }
}

















