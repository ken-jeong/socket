// cl ch14_broadcast_send.c /link ws2_32.lib
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#define BUF_SIZE 30

void ErrorHandling(char *message);

int main(void) {
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		ErrorHandling("WSAStartup() error");
	}
	
	SOCKET hSendSock = socket(PF_INET, SOCK_DGRAM, 0); // UDP

	BOOL bEnable = TRUE;
	setsockopt(hSendSock, SOL_SOCKET, SO_BROADCAST, (char*)&bEnable, sizeof(bEnable));

	// 0. 전송 목적 주소지 설정
	SOCKADDR_IN broadAddr;
	memset(&broadAddr, 0, sizeof(broadAddr));
	broadAddr.sin_family = AF_INET; // IPv4
	broadAddr.sin_port = htons(9000); // Big endian (Port 9000)
	broadAddr.sin_addr.s_addr = inet_addr("255.255.255.255"); // Broadcast Address	

	// 1. 파일 열기 (data.txt)
	FILE *fp;
	fp = fopen("data.txt", "r");
	if (fp == NULL) {
		printf("fopen() error");
		closesocket(hSendSock);
		WSACleanup();
		return 0;
	}

	// 2. 반복해서 파일 내용 send하기
	//    빈 파일 체크 (flag에 반영, 정상: 1, 비정상: 0)
	//    -> 비어있으면 while 건너뛰기 ...
	int flag = 1;
	char buf[BUF_SIZE];

	if (feof(fp)) {
		flag = 0;
	}
	
	while (flag == 1) {
		fgets(buf, BUF_SIZE, fp);
		sendto(hSendSock, buf, strlen(buf), 0, (SOCKADDR*)&broadAddr, sizeof(broadAddr));
		// 29바이트씩 전송 ...
		printf("%s", buf);
		Sleep(2000);

		if (feof(fp)) { // 파일을 다 읽은 경우 EOF 리턴
			fseek(fp, 0, SEEK_SET); // 파일 포인터를 맨 앞으로 이동
			// 처음부터 다시 파일 내용 읽기 위해
			printf("\n파일을 다 읽어서 맨 앞으로 포인터 이동");
		}
	}

	fclose(fp);
	closesocket(hSendSock);
	WSACleanup();
	return 0;
}

void ErrorHandling(char *message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
