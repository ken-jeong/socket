// 서버 프로그램.
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")

#define BUF_SIZE 30
void ErrorHandling(char *message);

int main()
{
	WSADATA wsaData;
	SOCKET servSock;
	char message[BUF_SIZE];
	int strLen;
	int clntAdrSz;
	
	SOCKADDR_IN servAdr, clntAdr;
	
	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!"); 
	
	servSock = socket(PF_INET, SOCK_DGRAM, 0);
	if(servSock == INVALID_SOCKET)
		ErrorHandling("UDP socket creation error");
	
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family=AF_INET;
	servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAdr.sin_port = htons(9000);
	
	if(bind(servSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
		ErrorHandling("bind() error");
	
	while(1) 
	{		
		clntAdrSz = sizeof(clntAdr);
		strLen = recvfrom(servSock, message, BUF_SIZE, 0, (SOCKADDR*)&clntAdr, &clntAdrSz);

		message[strLen] = '\0';
		printf("1. received msg(%s) from(IP:%s,PORT:%d)\n", message, 
			    inet_ntoa(clntAdr.sin_addr), ntohs(clntAdr.sin_port));

		printf("Server> Sleep 3 sec\n");
		Sleep(3000);

		strLen = recvfrom(servSock, message, BUF_SIZE, 0, (SOCKADDR*)&clntAdr, &clntAdrSz);
		message[strLen] = '\0';
		printf("2. received msg(%s) from(IP:%s,PORT:%d)\n", message, 
			    inet_ntoa(clntAdr.sin_addr), ntohs(clntAdr.sin_port));

		printf("Server> Sleep 3 sec\n");
		Sleep(3000);

		strLen = recvfrom(servSock, message, BUF_SIZE, 0, (SOCKADDR*)&clntAdr, &clntAdrSz);
		message[strLen] = '\0';
		printf("3. received msg(%s) from(IP:%s,PORT:%d)\n", message, 
			    inet_ntoa(clntAdr.sin_addr), ntohs(clntAdr.sin_port));		
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