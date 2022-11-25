#include <iostream>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

enum ePort { SERVER_PORT = 65456 };

#define _winsock_deprecated_no_warnings
using namespace std;

int main()
{
	cout << "> echo-client is activated" << endl;

	WSADATA wsaData;
	int iniResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iniResult != 0)
	{
		cerr << "Can't Initialize winsock! Quitiing" << endl;
		return -1;
	}

	SOCKET sock;
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		std::cerr << "Can't create socket, Err #" << WSAGetLastError() << std::endl;
		WSACleanup();
		return -1;
	}

	SOCKADDR_IN hint = {};
	hint.sin_family = AF_INET;
	hint.sin_port = htons(SERVER_PORT);
	hint.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int connResult = connect(sock, reinterpret_cast<sockaddr*>(&hint), sizeof(hint));

	if (connResult != 0) // connect
	{
		cout << "Can't connect to server, Err #" << WSAGetLastError() << endl;
		closesocket(sock);
		WSACleanup();
		return -1;
	}

	string cMessage;
	while (TRUE)
	{
		cin >> cMessage;
		if (send(sock, cMessage.c_str(), cMessage.size() + 1, 0) == SOCKET_ERROR)
		{
			cout << "> send() failed" << endl;
			break;
		}

		enum eBufSize { BUF_SIZE = 1024 };
		char buf[BUF_SIZE] = {};
		
		recv(sock, buf, BUF_SIZE, 0);
		cout << "> received: " << buf << endl;

		if (cMessage == "quit")
			break;
	}

	closesocket(sock);
	WSACleanup();

	cout << "> echo-client is de-activated" << endl;
	
	return 0;
}