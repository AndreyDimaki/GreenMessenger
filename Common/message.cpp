#include <sstream>
#include <regex>
#include <iostream>

#include "message.h"

Message::Message(int sender, int receiver, MessageTypeID messageID, const std::string& content)
{
    _ID = messageID;
    _content = content;
    _senderID = sender;
    _receiverID = receiver;
}

Message* Message::createNew(const std::string& inputString)
{
    std::regex messagePattern("[0-9]{1,8}:[0-9]{1,8}:[0-9]{1,2}:[0-9]{1,8}:[\\s\\S]{0,}");
    std::smatch messageMatch;
    if (std::regex_match(inputString, messageMatch, messagePattern))
    {
        std::regex numberPattern("[0-9]{1,}:");
        auto numberBegin = std::sregex_iterator(inputString.begin(), inputString.end(), numberPattern);
        std::vector<std::string> result(4);
        int messagePos = 0;
        for (int count = 0; count < 4; ++numberBegin, ++count)
        {
            std::smatch match = *numberBegin;
            result[count] = match.str();
            messagePos += result[count].size();
        }

        int senderID = std::stoi(result[0]);
        int receiverID = std::stoi(result[1]);
        MessageTypeID messageID = static_cast<MessageTypeID>(std::stoi(result[2]));
        std::string content = inputString.substr(messagePos, inputString.length()-messagePos-1);
        return new Message(senderID, receiverID, messageID, content);
    }
    return nullptr;
}

const std::string& Message::toOutputString() const
{
    std::stringstream ss;
    ss << _senderID << ':'
       << _receiverID << ':'
       << int(_ID) << ':'
       << _content.length() << ':'
       << _content << '\n';
    _outputString = ss.str();
    return _outputString;
}
