#include "SocketManager.h"

//public methods
//Ctor
SocketManager::SocketManager(unsigned short port) : SERVER_PORT(port) {
    memset(clientSockets, 0, sizeof(clientSockets));
    createSocket();
    createAddress();
    bindSocket();
    socketListen(10);
}

//异步处理多个连接 select函数
void SocketManager::socketSelect() {
    timeval tv;

    tv.tv_sec = 0;
    tv.tv_usec = 0;
    while (1) {
        myFD_SET();
        select(0, &fd, nullptr, nullptr, &tv);
        try {
            socketAccept();
            socketIO();
        } catch (MyException &e) {
            cout << e.get() << endl;
        }
    }
}

// private methods
// WSAStartup() link to the lib
void SocketManager::init() {
    WORD wVersionRequested;
    WSADATA wsaData;
    int ret;
    wVersionRequested = MAKEWORD(2, 2);
    ret = WSAStartup(wVersionRequested, &wsaData);
    if (ret != 0) {
        throw MyException("WSAStartup() failed!");
    }
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        WSACleanup();
        throw MyException("Invalid Winsock version!");
    }
}

// create a socket
void SocketManager::createSocket() {
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        WSACleanup();
        throw MyException("socket() failed!");
    }
}

void SocketManager::createAddress() {
    address.sin_family = AF_INET;
    address.sin_port = htons(SERVER_PORT);
    address.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
}

// bind a Socket
void SocketManager::bindSocket() {
    if (bind(serverSocket, (sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
        string errMessage = "bind() failed! code: " + WSAGetLastError();
        closesocket(serverSocket);
        WSACleanup();
        throw MyException(errMessage);
    }
}

void SocketManager::socketListen(int backlog) {
    if (listen(serverSocket, backlog) == SOCKET_ERROR) {
        string errMessage = "listen() failed! code: " + WSAGetLastError();
        closesocket(serverSocket);
        WSACleanup();
        throw MyException(errMessage);
    }
}

void SocketManager::myFD_SET() {
    FD_ZERO(&fd);
    FD_SET(serverSocket, &fd);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clientSockets[i] > 0) {
            FD_SET(clientSockets[i], &fd);
        }
    }
}

void SocketManager::socketAccept() {
    int nSize = sizeof(address);
    SOCKET acceptSocket;

    if (FD_ISSET(serverSocket, &fd)) {
        acceptSocket = accept(serverSocket, (sockaddr *)&address, &nSize);
        getpeername(acceptSocket, (sockaddr *)&address, &nSize);
        cout << "New connection , socket fd is " << acceptSocket
             << " , ip is : " << inet_ntoa(address.sin_addr)
             << ", port : " << ntohs(address.sin_port) << endl;
        addClientList(acceptSocket);
    }
}

void SocketManager::addClientList(SOCKET socketClient) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clientSockets[i] == 0) {
            clientSockets[i] = socketClient;
            return;
        }
    }
    //if the list is full
    throw MyException("add ClientList() failed! The list is full.");
}

void SocketManager::socketIO() {
    int lenRecv, len;
    bool firstRead;
    int nSize = sizeof(address);
    char buff[BUFF_LENGTH];
    char *ptr;
    SOCKET clientSocket;

    for (int i = 0; i < MAX_CLIENTS; i++) {
        clientSocket = clientSockets[i];
        if (FD_ISSET(clientSocket, &fd)) {
            len = LENGTH_BYTE;
            firstRead = true;
            memset(buff, 0, BUFF_LENGTH);
            ptr = buff;
            while (len > 0) {
                lenRecv = recv(clientSocket, ptr, BUFF_LENGTH, 0);
                if (lenRecv == SOCKET_ERROR) {
                    getpeername(clientSocket, (sockaddr *)&address, &nSize);
                    cout << "Recv() error, connection will be forcefully closed. socket fd is " << clientSocket
                         << " , ip is : " << inet_ntoa(address.sin_addr)
                         << ", port : " << ntohs(address.sin_port) << endl;
                    closesocket(clientSocket);
                    clientSockets[i] = 0;
                    break;
                } else if (lenRecv == 0) {
                    getpeername(clientSocket, (sockaddr *)&address, &nSize);
                    cout << "Connection closed, socket fd is " << clientSocket
                         << " , ip is : " << inet_ntoa(address.sin_addr)
                         << ", port : " << ntohs(address.sin_port) << endl;
                    closesocket(clientSocket);
                    clientSockets[i] = 0;
                    break;
                } else if (firstRead && (lenRecv >= LENGTH_BYTE || (ptr - buff) >= LENGTH_BYTE)) {
                    len = *(int *)buff - (ptr - buff);
                    firstRead = false;
                }
                len -= lenRecv;
                ptr += lenRecv;
            }
            if (len == 0) {
                response(i, buff);
            } else if (lenRecv != SOCKET_ERROR && lenRecv != 0) {
                throw MyException("Data missed!"); 
            }
        }
    }
}

void SocketManager::response(int index, char *buff) {
    string retStr;
    DataPack sdp;
    int op;

    op = buff[LENGTH_BYTE];
    retStr.clear();
    switch (op) {
        case 0:
            retStr = getTime();
            break;
        case 1:
            retStr = getPCName();
            break;
        case 2:
            retStr = getClientInfo();
            break;
        case 3:
            break;
        default:
            break;
    }
    sdp.len = sizeof(sdp);
    strcpy(sdp.data, retStr.c_str());
    if (send(clientSockets[index], (char *)&sdp, sdp.len, 0) == SOCKET_ERROR) {
        throw MyException("send() failed!");
    }
}

string SocketManager::getTime() {
    time_t now;
    string retStr;

    now = time(0);
    retStr = ctime(&now);
    retStr.erase(retStr.length() - 1, 1);
    return retStr;
}

string SocketManager::getPCName() {
    string retStr;
    char PCNameBuffer[50];

    gethostname(PCNameBuffer, sizeof(PCNameBuffer));
    retStr = PCNameBuffer;
    return retStr;
}

string SocketManager::getClientInfo() {
    string retStr;
    stringstream ss;
    int nSize = sizeof(address);
    int count = 0;

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clientSockets[i] > 0) {
            getpeername(clientSockets[i], (sockaddr *)&address, &nSize);
            ss << count++ << " " << clientSockets[i]
               << " " << inet_ntoa(address.sin_addr)
               << " " << ntohs(address.sin_port) << "\n";
        }
    }
    retStr = ss.str();
    return retStr;
}