#include <iostream>
#include <WinSock2.h>
using namespace std;

#pragma comment(lib, "ws2_32")

enum ePort { SERVER_PORT = 65456 };

#define _winsock_deprecated_no_warnings

int main()
{
	cout << "> echo-server is activated" << endl;

	WSADATA wsaData;
	int iniResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iniResult != 0)
	{
		cerr << "Can't Initialize winsock! Quitiing" << endl;
		return -1;
	}
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
	SOCKADDR_IN ClientSockInfo = {}; 
	int clientSize = sizeof(ClientSockInfo);

	// connection queue의 가장 앞에 있는 클라이언트 요청을 accept하고, client 소켓을 반환합니다.
	SOCKET clientSocket = accept(listeningSock, reinterpret_cast<sockaddr*>(&ClientSockInfo), &clientSize);
	if (clientSocket == INVALID_SOCKET)
	{
		cerr << "Can't accept a socket! Quitting" << endl;
		closesocket(listeningSock);
		WSACleanup();
		return -1;
	}
	in_addr in = ClientSockInfo.sin_addr;
	cout << "> client connected by IP address " << inet_ntoa(in) << " with Port number" << ClientSockInfo.sin_port << endl;

	while (TRUE)
	{
		enum eBufSize { BUF_SIZE = 1024 };
		char buf[BUF_SIZE] = {};
		int bytesReceived = recv(clientSocket, buf, BUF_SIZE, 0);

		cout << "> echoed: " << buf << endl;

		send(clientSocket, buf, strlen(buf), 0);
		//(send하는 소켓, 버퍼에 대한 포인터, 길이, 호출방법지정) .

		if (strcmp(buf, "quit") == 0)
			break;
	}

	closesocket(clientSocket);

	WSACleanup();

	cout << "> echo-server is de-activated" << endl;
	return 0;
}
