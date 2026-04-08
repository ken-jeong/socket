// 서버 프로그램.
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
//#include <ws2tcpip.h>
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")
#define  MAX_BUF_SIZE  1000
void ErrorHandling(char* message);

int main(void)
{  
	WSADATA		winsockData;
	char		Buffer[MAX_BUF_SIZE];
	int			returnValue, stopFlag = 0;
	SOCKET		serverSocket;

	printf("> start TCP server.\n");
	WSAStartup( MAKEWORD(2,2), &winsockData );
	serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN   serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family		= AF_INET;
	serverAddr.sin_port			= htons(9000); 
	serverAddr.sin_addr.s_addr	= inet_addr("127.0.0.1");
	//InetPton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

	bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));

	int backLog = 5; 
	listen(serverSocket, backLog );

	SOCKET			clientSocket;
	SOCKADDR_IN		clientAddr;
	int				addrLen;
	addrLen = sizeof(clientAddr);

	clientSocket = accept(serverSocket, (SOCKADDR*)&clientAddr, &addrLen);

	while(stopFlag == 0) {
		returnValue = recv(clientSocket, Buffer, MAX_BUF_SIZE, 0);
		if (returnValue > 0) {
			Buffer[returnValue] = '\0';
			printf("> recv: %s", Buffer);
		}else if (returnValue == SOCKET_ERROR) {
			ErrorHandling("recv() error");
		}else if (returnValue == 0) {
			stopFlag = 1;
		}
	}

	closesocket(serverSocket);
	WSACleanup();
	return 0;
}

void ErrorHandling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}