#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

static constexpr int16_t MessengerPort = 5025;

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

    const std::string& content() const
    {
        return _content;
    }

    int senderID() const
    {
        return _senderID;
    }

    int receiverID() const
    {
        return _receiverID;
    }

    static Message* createNew(const std::string& inputString);

    const std::string& toOutputString();

private:
    MessageTypeID _ID;
    std::string _content;
    std::string _outputString;
    int _senderID;
    int _receiverID;
};

#endif // MESSAGE_H
