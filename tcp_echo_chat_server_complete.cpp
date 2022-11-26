#include <iostream>
#include <WinSock2.h>
#include <list>

#pragma comment(lib, "ws2_32")


enum ePort { SERVER_PORT = 65456 };

#define _winsock_deprecated_no_warnings
using namespace std;

list<SOCKET> socket_list;

DWORD WINAPI makeThread(void* data)
{
	SOCKET socket = (SOCKET)data;
	char buffer[1024] = {};
	int receiveBytes;
	socket_list.push_back(socket);

	while (receiveBytes = recv(socket, buffer, 1024, 0))
	{
		if (receiveBytes > 0)
		{
			cout << "> received ( " << buffer << " ) by Thread-" << socket;
			cout << "and Echoed to " << socket_list.size() << " clients" << endl;

			for (list<SOCKET>::iterator it = socket_list.begin(); it != socket_list.end(); ++it)
			{
				send(*it, buffer, strlen(buffer), 0);
			}

			if (strcmp(buffer, "quit") == 0)
				break;
		}
		else
		{
			break;
		}
	}

	socket_list.remove(socket);
	closesocket(socket);
	cout << "> active threads are remained : " << socket_list.size() << endl;

	if (socket_list.empty()) {
		cout << "stop procedure started" << endl;
		cout << "> echo-server is de-activated" << endl;
		exit(100);
	}
		

	return 0;
}

int main()
{
	cout << "> echo-server-multithread is activated" << endl;

	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	//socket()
	SOCKET listeningSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // 3번 째 인자 0도 OK
	if (listeningSock == INVALID_SOCKET)
	{
		cerr << "Can't create a socket! Quitting" << endl;
		WSACleanup();
		return -1;
	}

	//bind()
	SOCKADDR_IN hint = {}; //초기화 권장사항
	hint.sin_family = AF_INET;
	hint.sin_port = htons(SERVER_PORT);
	hint.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int bindResult = bind(listeningSock, reinterpret_cast<sockaddr*>(&hint), sizeof(hint));

	if (bindResult != 0)  //bind()
	{
		cerr << "Can't bind a socket! Quitting" << endl;
		closesocket(listeningSock);
		WSACleanup();
		return -1;
	}

	int listenResult = listen(listeningSock, SOMAXCONN);
	if (listenResult != 0)
	{
		cerr << "Can't listen a socket! Quitting" << endl;
		closesocket(listeningSock);
		WSACleanup();
		return -1;
	}

	// accept
	sockaddr_in address = {};
	int size = sizeof(address);
	SOCKET client;

	HANDLE hThread;

	while (TRUE)
	{
		client = accept(listeningSock, (sockaddr*)&address, &size);
		cout << "> client connected by IP address " << inet_ntoa(address.sin_addr) << " with Port number" << address.sin_port << endl;

		hThread = CreateThread(NULL, 0, makeThread, (void*)client, 0, NULL);

		CloseHandle(hThread);

	}

	closesocket(listeningSock);

	WSACleanup();

	cout << "> echo-server is de-activated" << endl;
	return 0;
}