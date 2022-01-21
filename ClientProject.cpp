#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include"iostream"
using namespace std;
#pragma comment(lib,"ws2_32.lib")

#define RECV_OVER 1
#define RECV_YET 0
int iStatus = RECV_YET;
//接受数据
unsigned __stdcall ThreadRecv(void* param)
{
    char buf[128] = { 0 };
    while (1)
    {
        int ret = recv(*(SOCKET*)param, buf, sizeof(buf), 0);
        if (ret == SOCKET_ERROR)
        {
            Sleep(500);
            continue;
        }
        if (strlen(buf) != 0)
        {
            cout << "服务器:" << buf << endl;
            iStatus = RECV_OVER;
        }
        else
            Sleep(100);
    }
    return 0;
}

//发送数据
unsigned __stdcall ThreadSend(void* param)
{
    char buf[128] = { 0 };
    int ret = 0;
    while (1)
    {
        int c = _getch();
        cout << "客户端:";
        gets_s(buf);
        ret = send(*(SOCKET*)param, buf, sizeof(buf), 0);
        if (ret == SOCKET_ERROR)
            return 1;
    }
    return 0;
}

int main()
{
    cout << "客户端" << endl << endl;

    //存储套接字信息结构
    WSADATA wsaData = { 0 };
    SOCKET ServerSocket = INVALID_SOCKET;//服务器套接字
    SOCKET ClientSocket = INVALID_SOCKET;//客户端套接字
    SOCKADDR_IN ServerAddr = { 0 };//服务器地址
    SOCKADDR_IN ClientAddr = { 0 };//客户端地址
    int iClientAddrLen = sizeof(ClientAddr);

    //初始化套接字
    if (WSAStartup(MAKEWORD(2, 2), &wsaData))
    {
        cout << "套接字初始化失败!  错误代码:" << WSAGetLastError() << endl;
        return -1;
    }
    //判断套接字版本
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
    {
        cout << "套接字不是 2.2 版本!" << endl;
        return -1;
    }

    //创建套接字
    ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ClientSocket == INVALID_SOCKET)
    {
        cout << "套接字创建失败!  错误代码:" << WSAGetLastError() << endl;
        return -1;
    }

    int port;
    char addr[4096] = { 0 };
    cout << "请输入服务器IP地址:";
    gets_s(addr);
    cout << "请输入服务器端口号:";
    cin >> port;
    getchar();

    cout << "连接中..." << endl;

    //设置服务器地址
    ClientAddr.sin_family = AF_INET;//连接方式
    ClientAddr.sin_port = htons(port);
    ClientAddr.sin_addr.S_un.S_addr = inet_addr(addr);

    if (connect(ClientSocket, (SOCKADDR*)&ClientAddr, sizeof(ClientAddr)) < 0)
    {
        cout << "连接失败!  错误代码:" << WSAGetLastError() << endl;
        closesocket(ClientSocket);
        WSACleanup();
        return -1;
    }
    
    cout << "连接成功!" << endl;
    cout << endl;

    _beginthreadex(NULL, 0, ThreadRecv, &ClientSocket, 0, NULL); //接收消息线程
    _beginthreadex(NULL, 0, ThreadSend, &ClientSocket, 0, NULL); //发送消息线程
    for (int k = 0;k < 1000;k++)//主线程休眠
    {
        Sleep(10000000);
    }

    closesocket(ClientSocket);
    WSACleanup();
    system("pause");
    return 0;
}


