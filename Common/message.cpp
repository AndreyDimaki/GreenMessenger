#include <sstream>
#include <regex>

#include "message.h"

Message::Message(int sender, int receiver, MessageTypeID messageID, const std::string &content)
{
    _ID = messageID;
    _content = std::move(content);
    _senderID = sender;
    _receiverID = receiver;
}

Message* Message::createNew(const std::string& inputString)
{
    std::regex messagePattern("[0-9]{1,}:[0-9]{1,}:[0-9]{1,}:[0-9]{1,}:[\\s\\S]{0,}");
    std::smatch messageMatch;
    if (std::regex_match(inputString, messageMatch, messagePattern))
    {
        MessageTypeID messageID;
        int senderID;
        int receiverID;
        std::string content;
        /// TODO: Здесь формируем экземпляр распарсенного сообщения
        return new Message(0, 0, MessageTypeID::MSG_SendMessage, "");
    }
    return nullptr;
}

const std::string& Message::toOutputString()
{
    std::stringstream ss;
    ss << int(_ID) << ':'
       << _senderID << ':'
       << _receiverID << ':'
       << _content.length() << ':'
       << _content << '\n';
    _outputString = ss.str();
    return _outputString;
}
