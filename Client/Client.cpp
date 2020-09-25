//informWinClient.cpp：参数为serverIP name age
#include <stdio.h>
#include <winsock2.h>

#include <iostream>

#define SERVER_PORT 6666  //侦听端口

using namespace std;

//客户端向服务器传送的结构：
struct student {
    int len;
    char op;
    char name[100];
    int age;
};

int main(int argc, char *argv[]) {
    WORD wVersionRequested;
    WSADATA wsaData;
    int ret;
    SOCKET sClient;               //连接套接字
    struct sockaddr_in saServer;  //地址信息
    struct student stu;
    char *ptr = (char *)&stu;
    BOOL fSuccess = TRUE;

    //WinSock初始化：
    wVersionRequested = MAKEWORD(2, 2);  //希望使用的WinSock DLL的版本
    ret = WSAStartup(wVersionRequested, &wsaData);
    if (ret != 0) {
        printf("WSAStartup() failed!\n");
        return 0;
    }
    //确认WinSock DLL支持版本2.2：
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        WSACleanup();
        printf("Invalid Winsock version!\n");
        return 0;
    }

    //创建socket，使用TCP协议：
    sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sClient == INVALID_SOCKET) {
        WSACleanup();
        printf("socket() failed!\n");
        return 0;
    }

    //构建服务器地址信息：
    saServer.sin_family = AF_INET;           //地址家族
    saServer.sin_port = htons(SERVER_PORT);  //注意转化为网络字节序
    saServer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

    //连接服务器：
    ret = connect(sClient, (struct sockaddr *)&saServer, sizeof(saServer));
    if (ret == SOCKET_ERROR) {
        printf("connect() failed!\n");
        closesocket(sClient);  //关闭套接字
        WSACleanup();
        return 0;
    }
    system("pause");
    stu.len = sizeof(student);
    stu.op = 2;
    strcpy(stu.name, "tmy");
    stu.age = 18;
    ret = send(sClient, (char*)&stu, stu.len, 0);
    if (ret == SOCKET_ERROR) {
        printf("send() failed!\n");
    } else
        printf("student info has been sent!\n");
    char buff[150];
    ret = recv(sClient, buff, sizeof(buff), 0);
    cout << buff + 5 << endl;
    closesocket(sClient);  //关闭套接字
    WSACleanup();
}
