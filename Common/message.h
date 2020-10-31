#ifndef MESSAGE_H
#define MESSAGE_H

#include <QObject>

static constexpr quint16 MessengerPort = 5025;

enum class MessageTypeID : int
{
    MSG_CreateNewUser = 1,
    MSG_LogIn,
    MSG_SendMessage,
    MSG_ReceiveUserList,
    MSG_ReceiveHistory,
    MSG_CannotCreateUser,
    MSG_CannotLogin
};

class Message
{
public:
    explicit Message(int sender, int receiver, MessageTypeID messageID, const std::string& content);

    MessageTypeID id() const
    {
        return _ID;
    }

    const std::string& content()
    {
        return _content;
    }

    int senderID()
    {
        return _senderID;
    }

    int receiverID()
    {
        return _receiverID;
    }

private:
    MessageTypeID _ID;
    std::string _content;
    int _senderID;
    int _receiverID;
};

#endif // MESSAGE_H
