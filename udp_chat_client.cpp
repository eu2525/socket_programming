#include <iostream>
#include <WinSock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32")

enum ePort { SERVER_PORT = 65456 };

#define _winsock_deprecated_no_warnings
using namespace std;

SOCKADDR_IN serverAddr = {};
string cMessage;

DWORD WINAPI makeThread(void* data)
{
    SOCKET socket = (SOCKET)data;
    enum eBufSize { BUF_SIZE = 1024 };
    char buffer[BUF_SIZE] = {};
    int receiveBytes;
    int slen = sizeof(SOCKADDR_IN);

    while (receiveBytes = recvfrom(socket, buffer, BUF_SIZE, 0, (sockaddr*)&serverAddr, &slen))
    {
        if (receiveBytes > 0)
        {
            cout << "> received: " << buffer << endl;
            recvfrom(socket, buffer, strlen(buffer), 0, (sockaddr*)&serverAddr, &slen);
        }
        else
        {
            break;
        }
    }

    return 0;
}
DWORD WINAPI makeSendThread(void* data)
{

    SOCKET socket = (SOCKET)data;

    while (TRUE)
    {

        cin >> cMessage;
        if (sendto(socket, cMessage.c_str(), cMessage.size() + 1, 0, (sockaddr*)&serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
        {
            cout << "> send() failed" << endl;
            break;
        }
        if (cMessage == "quit")
        {
            exit(0);
        }
    }

    return 0;
}



int main() {
    cout << "> echo-client is activated" << endl;

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) // 家南 汲沥 (家南 滚傈, 备炼眉 林家)
    {
        cout << "> WSAStartup() failed" << endl;
        return -1;
    }


    SOCKET m_clientSock;
    m_clientSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

    HANDLE hThread;
    HANDLE sendThread;

    string cMessage;
    while (TRUE)
    {
        sendThread = CreateThread(NULL, 0, makeSendThread, (void*)m_clientSock, 0, NULL);
        CloseHandle(sendThread);

        hThread = CreateThread(NULL, 0, makeThread, (void*)m_clientSock, 0, NULL);
        CloseHandle(hThread);
    }

    closesocket(m_clientSock); //家南 摧扁
    WSACleanup();

    cout << "> echo-server is de-activated" << endl;
    return 0;
}