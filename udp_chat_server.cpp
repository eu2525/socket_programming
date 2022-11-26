#include <iostream>
#include <WinSock2.h>
#include <list>

#pragma comment(lib, "ws2_32")


enum ePort { SERVER_PORT = 65456 };

#define _winsock_deprecated_no_warnings
using namespace std;

list<SOCKADDR_IN> socket_list;

DWORD WINAPI makeThread(void* data)
{
	SOCKADDR_IN m_ClientInfo;
	SOCKET socket = (SOCKET)data;
	char buffer[1024] = {};
	int nClient_Size = sizeof(m_ClientInfo);
	int nRecvLen;
	while (nRecvLen = recvfrom(socket, reinterpret_cast<char*>(buffer), 1024, 0, (SOCKADDR*)&m_ClientInfo, &nClient_Size))
	{
		if (nRecvLen > 0)
		{
			if (strcmp(buffer, "#REG") == 0)
			{
				socket_list.push_back(m_ClientInfo);
				cout << "> client registered: ('" << inet_ntoa(m_ClientInfo.sin_addr) << "', " << ntohs(m_ClientInfo.sin_port) << ") current clients: " << socket_list.size() << endl;
			}
			else if (strcmp(buffer, "#DEREG") == 0 || strcmp(buffer, "quit") == 0)
			{

				list<SOCKADDR_IN>::iterator rmit;
				for (list<SOCKADDR_IN>::iterator it = socket_list.begin(); it != socket_list.end(); ++it)
				{
					if ((*it).sin_port == m_ClientInfo.sin_port)
					{
						rmit = it;
					}
				}
				socket_list.erase(rmit);
				cout << "> client de-registered: ('" << inet_ntoa(m_ClientInfo.sin_addr) << "', " << ntohs(m_ClientInfo.sin_port) << ") current clients: " << socket_list.size() << endl;
			}
			else {
				if (socket_list.size() == 0)
				{
					cout << "> no clients to echo" << endl;
				}
				else
				{
					int same_count = 0;
					for (list<SOCKADDR_IN>::iterator it = socket_list.begin(); it != socket_list.end(); ++it)
					{
						if ((*it).sin_port == m_ClientInfo.sin_port)
						{
							same_count++;
						}
					}
					if (same_count != 0) {
						cout << "> received: (" << buffer << ") and echoed to " << socket_list.size() << " clients" << endl;
						for (list<SOCKADDR_IN>::iterator it = socket_list.begin(); it != socket_list.end(); ++it)
						{
							sendto(socket, buffer, strlen(buffer), 0, (sockaddr*)&((*it)), sizeof(SOCKADDR_IN));
						
						}
						cout << endl;
					}
					else {
						cout << "> no clients to echo" << endl;
					}
				}
			}
		}
		else
		{
			break;
		}
	}
	closesocket(socket);
	return 0;
}

int main()
{
	cout << "> echo-server-multithread is activated" << endl;

	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	//socket()
	SOCKET m_ServerSocket;
	m_ServerSocket = socket(AF_INET, SOCK_DGRAM, 0);

	//bind()
	SOCKADDR_IN hint = {}; //초기화 권장사항
	hint.sin_family = AF_INET;
	hint.sin_port = htons(SERVER_PORT);
	hint.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int bindResult = bind(m_ServerSocket, reinterpret_cast<sockaddr*>(&hint), sizeof(hint));

	if (bindResult != 0)  //bind()
	{
		cerr << "Can't bind a socket! Quitting" << endl;
		closesocket(m_ServerSocket);
		WSACleanup();
		return -1;
	}

	HANDLE hThread;

	while (TRUE)
	{
		hThread = CreateThread(NULL, 0, makeThread, (void*)m_ServerSocket, 0, NULL);

		CloseHandle(hThread);
	}

	closesocket(m_ServerSocket);

	WSACleanup();

	cout << "> echo-server is de-activated" << endl;
	return 0;
}