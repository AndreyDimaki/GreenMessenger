#ifndef USER_H
#define USER_H

#include <QObject>

class User
{
public:
    explicit User(int id, const std::string& name);

    int id() const
    {
        return _ID;
    }

    const std::string& name() const
    {
        return _name;
    }

private:
    int _ID;
    std::string _name;

};

#endif // USER_H
