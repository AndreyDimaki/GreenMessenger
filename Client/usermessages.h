#ifndef USERMESSAGES_H
#define USERMESSAGES_H

#include "../Common/message.h"
#include "../Common/user.h"

class UserMessages : public User
{
public:
    UserMessages(int id, const std::string& name);

    const QList<Message>& messages() const
    {
        return _messages;
    }

    void appendMessage(const Message* message);

    void clear();

private:
    QList<Message> _messages;
};

#endif // USERMESSAGES_H
