#include "usermessages.h"

UserMessages::UserMessages(int id, const std::string& name) :
    User(id, name, nullptr)
{

}

void UserMessages::appendMessage(const Message *message)
{
    _messages.push_back(*message);
}

void UserMessages::clear()
{
    _messages.clear();
}
