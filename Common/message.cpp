#include "message.h"

Message::Message(int sender, int receiver, MessageTypeID messageID, const std::string &content)
{
    _ID = messageID;
    _content = std::move(content);
    _senderID = sender;
    _receiverID = receiver;
}
