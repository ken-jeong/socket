// 클라이언트 프로그램.
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#define BUF_SIZE 100
void ErrorHandling(char *message);
 
int main(int argc, char *argv[])
{
	WSADATA		wsaData;
	SOCKET		servSock;
	char		message[BUF_SIZE];
	int			strLen, rcvLen;
	int			clntAdrSz, index=0;
	
	SOCKADDR_IN servAdr, clntAdr;
	
	if(WSAStartup(MAKEWORD(2, 2), &wsaData)!=0)
		ErrorHandling("WSAStartup() error!"); 
	
	servSock = socket(PF_INET, SOCK_DGRAM, 0);
	if(servSock == INVALID_SOCKET)
		ErrorHandling("UDP socket creation error");
	
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family		= AF_INET;
	servAdr.sin_addr.s_addr	= htonl(INADDR_ANY);
	servAdr.sin_port		= htons(9000);
	
	if(bind(servSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
		ErrorHandling("bind() error");
	
	printf("> Waiting UDP message.\n");

	while(1) 
	{ 
		clntAdrSz = sizeof( clntAdr );
		rcvLen = recvfrom( servSock, message, sizeof(message), 0,
						   (SOCKADDR*)&clntAdr, &clntAdrSz );
		message[rcvLen] = 0;
		strLen = strlen(message);
		printf("%d> message from client(IP:%s,PORT:%d) : %s (%d bytes)\n", ++index,
			   inet_ntoa(clntAdr.sin_addr), ntohs(clntAdr.sin_port), message, strLen );

		Sleep(3000);
	}	
	closesocket(servSock);
	WSACleanup();
	return 0;
}

void ErrorHandling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}