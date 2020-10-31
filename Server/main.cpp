#include <iostream>

#include <QCoreApplication>

#include "MessengerServer.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    MessengerServer server(nullptr);

    std::cout << "Green Messenger Server is on... Enjoy!" << std::endl;

    return a.exec();
}
