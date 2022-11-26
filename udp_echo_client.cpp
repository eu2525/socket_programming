#include <iostream>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32")


enum ePort { SERVER_PORT = 65456 };

#define _winsock_deprecated_no_warnings
using namespace std;

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

    SOCKADDR_IN serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

    int nSize = sizeof(serverAddr);
    int sendSize = 0;

    string cMessage;
    while (TRUE)
    {
        cin >> cMessage;
        if (sendto(m_clientSock, cMessage.c_str(), cMessage.size() + 1, 0, (sockaddr*)&serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
        {
            cout << "> send() failed" << endl;
            break;
        }
        enum eBufSize { BUF_SIZE = 1024 };
        char buf[BUF_SIZE] = {};
        int slen = sizeof(SOCKADDR_IN);
        recvfrom(m_clientSock, buf, BUF_SIZE, 0, (sockaddr*)&serverAddr, &slen);
        cout << "> received: " << buf << endl;

        if (cMessage == "quit")
            break;
    }

    closesocket(m_clientSock); //家南 摧扁
    WSACleanup();

    cout << "> echo-server is de-activated" << endl;
    return 0;
}
