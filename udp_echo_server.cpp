#include <iostream>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32")


enum ePort { SERVER_PORT = 65456 };

#define _winsock_deprecated_no_warnings
using namespace std;

int main() {

    SOCKADDR_IN m_ClientInfo;

    WSADATA wsa; // Winsock 데이터 구조체

    int retVal = 0;
    int nRecvLen = 0;
    char Buffer[1024] = { 0, };

    cout << "> echo-server is activated" << endl;

    //WS2_32.DLL 초기화 
    if (WSAStartup(MAKEWORD(2, 2), &wsa) == SOCKET_ERROR)
    {
        WSACleanup();
        return -1;
    }

    // UDP Socket 생성 (SOCK_DGRAM : UDP)    
    SOCKET m_ServerSocket;
    m_ServerSocket = socket(AF_INET, SOCK_DGRAM, 0);

    SOCKADDR_IN m_ServerInfo = {};
    m_ServerInfo.sin_family = AF_INET; // IPv4 
    m_ServerInfo.sin_addr.s_addr = inet_addr("127.0.0.1"); //Local 
    m_ServerInfo.sin_port = htons(SERVER_PORT);

    retVal = bind(m_ServerSocket, (SOCKADDR*)&m_ServerInfo, sizeof(m_ServerInfo));

    if (retVal == SOCKET_ERROR)
        return -1;

    int nClient_Size = sizeof(m_ClientInfo);

    while (true)
    { // Recv 대기. Send한 Client 정보 저장 
        nRecvLen = recvfrom(m_ServerSocket, reinterpret_cast<char*>(Buffer), 1024, 0, (SOCKADDR*)&m_ClientInfo, &nClient_Size);

        if (nRecvLen > 0)
        {
            cout << "> Received from [ ADDR: " << inet_ntoa(m_ClientInfo.sin_addr) << ", PORT: " << ntohs(m_ClientInfo.sin_port) << " ]" << endl;
            cout << "> echoed: " << Buffer << endl;

            sendto(m_ServerSocket, Buffer, strlen(Buffer), 0, (sockaddr*)&m_ClientInfo, sizeof(SOCKADDR_IN));
        }
        else
        {
            break;
        }
    }

    closesocket(m_ServerSocket);
    WSACleanup();

    cout << "> echo-server is de-activated" << endl;
    return 0;

}
