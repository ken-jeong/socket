// 클라이언트 프로그램.
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
	SOCKET sock;
	char message[BUF_SIZE];
	int strLen;
	
	SOCKADDR_IN servAdr;
	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!"); 

	sock = socket(PF_INET, SOCK_DGRAM, 0);   
	if(sock == INVALID_SOCKET)
		ErrorHandling("socket() error");
	
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servAdr.sin_port = htons(9000);
	
	//connect(sock, (SOCKADDR*)&servAdr, sizeof(servAdr));

	while(1)
	{
		printf("Insert message(q to quit): ");
		scanf("%s", message);
		if(!strcmp(message,"q") || !strcmp(message,"Q"))	
			break;

		//send(sock, message, strlen(message), 0);
		sendto(sock, message, strlen(message), 0, (SOCKADDR*)&servAdr, sizeof(servAdr));
		printf("1. sent msg(%s) to(IP:%s,PORT:%d)\n", message, inet_ntoa(servAdr.sin_addr), ntohs(servAdr.sin_port));

		//send(sock, message, strlen(message), 0);
		sendto(sock, message, strlen(message), 0, (SOCKADDR*)&servAdr, sizeof(servAdr));
		printf("2. sent msg(%s) to(IP:%s,PORT:%d)\n", message, inet_ntoa(servAdr.sin_addr), ntohs(servAdr.sin_port));

		//send(sock, message, strlen(message), 0);
		sendto(sock, message, strlen(message), 0, (SOCKADDR*)&servAdr, sizeof(servAdr));
		printf("3. sent msg(%s) to(IP:%s,PORT:%d)\n", message, inet_ntoa(servAdr.sin_addr), ntohs(servAdr.sin_port));		
	}

	closesocket(sock);
	WSACleanup();
	return 0;
}

void ErrorHandling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
