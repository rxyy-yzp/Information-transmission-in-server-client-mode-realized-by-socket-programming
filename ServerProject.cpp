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

unsigned __stdcall ThreadRecv(void* param)//接受数据
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
            cout << "客户端:" << buf << endl;
            iStatus = RECV_OVER;
        }
        else
            Sleep(100);
    }
    return 0;
}
unsigned __stdcall ThreadSend(void* param)//发送数据
{
    char buf[128] = { 0 };
    int ret = 0;
    while (1)
    {
        int c = _getch();
        cout << "服务器:";
        gets_s(buf);
        ret = send(*(SOCKET*)param, buf, sizeof(buf), 0);
        if (ret == SOCKET_ERROR)
            return 1;
    }
    return 0;
}

int main()
{
    cout << "服务器" << endl << endl;

    //存储套接字信息结构
    WSADATA wsaData = { 0 };
    SOCKET ServerSocket = INVALID_SOCKET;//服务器套接字
    SOCKET ClientSocket = INVALID_SOCKET;//客户端套接字
    SOCKADDR_IN ServerAddr = { 0 };//服务器地址
    SOCKADDR_IN ClientAddr = { 0 };//客户端地址
    int iClientAddrLen = sizeof(ClientAddr);
    USHORT uPort = 8200;//服务器监听端口号

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
    ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ServerSocket == INVALID_SOCKET)
    {
        cout << "套接字创建失败!  错误代码:" << WSAGetLastError() << endl;
        return -1;
    }

    //设置服务器地址
    ServerAddr.sin_family = AF_INET;//连接方式
    ServerAddr.sin_port = htons(uPort);//服务器监听端口号
    ServerAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//任何客户端都可以连接该服务器

    //绑定服务器
    if (SOCKET_ERROR == bind(ServerSocket, (SOCKADDR*)&ServerAddr, sizeof(ServerAddr)))
    {
        cout<<"绑定失败!  错误代码:"<< WSAGetLastError() << endl;
        closesocket(ServerSocket);
        return -1;
    }

    //监听是否有客户端连接
    if (SOCKET_ERROR == listen(ServerSocket, 10))
    {
        cout << "监听失败!  错误代码:" << WSAGetLastError() << endl;
        closesocket(ServerSocket);
        WSACleanup();
        return -1;
    }

    char message[4096] = "服务器";
    cout << "服务器正在等待连接..." << endl;

    //如果客户端有连接请求，则接受
    ClientSocket = accept(ServerSocket, (SOCKADDR*)&ClientAddr, &iClientAddrLen);
    if (ClientSocket == INVALID_SOCKET)
    {
        cout << "连接失败!  错误代码:" << WSAGetLastError() << endl;
        closesocket(ServerSocket);
        WSACleanup();
        return -1;
    }
    cout << "连接成功!  客户端IP地址:" <<
        inet_ntoa(ClientAddr.sin_addr) << "    端口号:" << htons(ClientAddr.sin_port) << endl;
    cout << endl;

    _beginthreadex(NULL, 0, ThreadRecv, &ClientSocket, 0, NULL); //接收消息线程
    _beginthreadex(NULL, 0, ThreadSend, &ClientSocket, 0, NULL); //发送消息线程
    for (int k = 0;k < 1000;k++)//主线程休眠
    {
        Sleep(10000000);
    }

    //关闭套接字
    closesocket(ClientSocket);
    closesocket(ServerSocket);
    WSACleanup();

    return 0;
}


