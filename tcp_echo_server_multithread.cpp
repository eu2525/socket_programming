#include <iostream>
#include <WinSock2.h>
using namespace std;

#pragma comment(lib, "ws2_32")

enum ePort { SERVER_PORT = 65456 };

#define _winsock_deprecated_no_warnings

int sendall(int s, char* buf, int* len)
{
	int total = 0; // how many bytes we've sent
	int bytesleft = *len; // how many we have left to send
	int n = 0;
	while (total < *len) {
		n = send(s, buf + total, bytesleft, 0);
		if (n == -1) {
			/* print/log error details */
			break;
		}
		total += n;
		bytesleft -= n;
	}
	*len = total; // return number actually sent here
	return n == -1 ? -1 : 0; // return -1 on failure, 0 on success
}


int main()
{
	cout << "> echo-server-multithread is activated" << endl;

	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	//socket()
	SOCKET listeningSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // 3�� ° ���� 0�� OK
	if (listeningSock == INVALID_SOCKET)
	{
		cerr << "Can't create a socket! Quitting" << endl;
		WSACleanup();
		return -1;
	}

	//bind()
	SOCKADDR_IN hint = {}; //�ʱ�ȭ �������
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

	WSAEVENT wsaEvent = WSACreateEvent();
	WSAEventSelect(listeningSock, wsaEvent, FD_ACCEPT);

	SOCKET sockets[WSA_MAXIMUM_WAIT_EVENTS];
	WSAEVENT events[WSA_MAXIMUM_WAIT_EVENTS];
	sockets[0] = listeningSock;
	events[0] = wsaEvent;
	int counter = 1;

	while (true) {
		DWORD res = WSAWaitForMultipleEvents(counter, events, FALSE, WSA_INFINITE, TRUE);

		if (res == WSA_WAIT_FAILED)
			break;

		int idx = res - WSA_WAIT_EVENT_0;

		WSANETWORKEVENTS networkEvents;
		if (WSAEnumNetworkEvents(sockets[idx], events[idx], &networkEvents) == SOCKET_ERROR)
			break;

		//accept�� socket�� event ����
		if (networkEvents.lNetworkEvents & FD_ACCEPT) {
			sockaddr_in address = {};
			int size = sizeof(address);
			SOCKET client = accept(sockets[idx], (sockaddr*)&address, &size);
			
			cout << "> client connected by IP address " << inet_ntoa(address.sin_addr) << " with Port nubmer" << address.sin_port << endl;


			WSAEVENT cEvt = WSACreateEvent();
			WSAEventSelect(client, cEvt, FD_READ | FD_CLOSE);

			sockets[counter] = client;
			events[counter] = cEvt;
			counter++;
		}
		//recv�� send------------------------
		if (networkEvents.lNetworkEvents & FD_READ) {
			char buffer[128];
			int readBytes = recv(sockets[idx], buffer, 128, 0);
			if (readBytes > 0)
			{
				cout << "> echoed: " << buffer << " by Thread-" << sockets[idx] << endl;

				int a = send(sockets[idx], buffer, strlen(buffer), 0);

				if (strcmp(buffer, "quit") == 0)
					break;
			}
		}

		//close
		if (networkEvents.lNetworkEvents & FD_CLOSE) {
			closesocket(sockets[idx]);
			WSACloseEvent(events[idx]);
			sockets[idx] = sockets[counter - 1];
			events[idx] = events[counter - 1];
			counter--;
		}
	}

	closesocket(listeningSock);
	WSACloseEvent(wsaEvent);
	WSACleanup();

	cout << "> echo-server-multithread is de-activated" << endl;
	return 0;
}