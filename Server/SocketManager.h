#ifndef SOCKETMANAGER_H
#define SOCKETMANAGER_H

#include <winsock2.h>

#include <ctime>
#include <iostream>
#include <sstream>

#include "Exception.h"
#include "DataPack.h"

using namespace std;

#define MAX_CLIENTS 30


class SocketManager {
private:
    SOCKET serverSocket;
    sockaddr_in address;
    const unsigned short SERVER_PORT;
    SOCKET clientSockets[MAX_CLIENTS];
    fd_set fd;

public:
    static void init();

    SocketManager(unsigned short port = 6666);
    void socketSelect();

private:
    void createSocket();
    void createAddress();
    void bindSocket();
    void socketListen(int backlog);

    void myFD_SET();
    void socketAccept();
    void addClientList(SOCKET socketClient);
    void socketIO();

    void response(int index, char *buff);
    string getTime();
    string getPCName();
    string getClientInfo();
};

#endif