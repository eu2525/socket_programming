#include <iostream>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

enum ePort { SERVER_PORT = 65456 };

#define _winsock_deprecated_no_warnings
using namespace std;


DWORD WINAPI makeThread(void* data)
{
	SOCKET socket = (SOCKET)data;
	enum eBufSize { BUF_SIZE = 1024 };
	char buffer[BUF_SIZE] = {};
	int readBytes = recv(socket, buffer, 1024, 0);

	while (readBytes != 0)
	{
		if (readBytes > 0)
		{
			cout << "> received: " << buffer << endl;

			recv(socket, buffer, strlen(buffer), 0);

		}
		else
		{
			break;
		}
	}

	closesocket(socket);
	return 0;
}

DWORD WINAPI makeSendThread(void* data)
{
	SOCKET socket = (SOCKET)data;

	while (TRUE)
	{
		string cMessage;

		cin >> cMessage;
		if (send(socket, cMessage.c_str(), cMessage.size() + 1, 0) == SOCKET_ERROR)
		{
			cout << "> send() failed" << endl;
			break;
		}

		if (cMessage == "quit")
		{
			exit(0);
		}
	}

	closesocket(socket);
	return 0;
}


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
		cout << "Can't connect, Err #" << WSAGetLastError() << endl;
		closesocket(sock);
		WSACleanup();
		return -1;
	}

	HANDLE hThread;
	HANDLE sendThread;

	while (TRUE)
	{
		hThread = CreateThread(NULL, 0, makeThread, (void*)sock, 0, NULL);
		sendThread = CreateThread(NULL, 0, makeSendThread, (void*)sock, 0, NULL);

		CloseHandle(hThread);
		CloseHandle(sendThread);

	}

	closesocket(sock);
	WSACleanup();

	cout << "> echo-client-chat is de-activated" << endl;

	return 0;
}