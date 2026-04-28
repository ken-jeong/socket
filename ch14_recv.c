#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h> // for struct ip_mreq
#pragma comment(lib, "Ws2_32.lib")

#define BUF_SIZE 30

void ErrorHandling(char *message);

int main(void) {
	WSADATA			wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		ErrorHandling("WSAStartup() error");
	}

	SOCKET hRecvSock = socket(PF_INET, SOCK_DGRAM, 0); // UDP

	// 1. 서버 주소 binding
	SOCKADDR_IN Addr;
	memset(&Addr, 0, sizeof(Addr));
	Addr.sin_family = AF_INET; // IPv4
	Addr.sin_port = htons(9000); // Big endian (Port 9000)
	Addr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(hRecvSock, (SOCKADDR*)&Addr, sizeof(Addr));
	
	// 2. 멀티캐스트 주소 수신하도록 설정하기: 소켓 옵션 사용
	struct ip_mreq joinAddr;
	joinAddr.im_multiaddr.s_addr = inet_addr("224.1.1.2"); // Class D Multicast Address
	joinAddr.im_interface.s_addr = htonl(INADDR_ANY); // IP 주소 알아서 설정
	
	setsockopt(hRecvSock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void*)&joinAddr, sizeof(joinAddr));
	
	// 3. 반복적으로 수신하기 -> 수신 여부 확인
	char buf[BUF_SIZE];

	while (1) {
		int strLen = recvfrom(hRecvSock, buf, BUF_SIZE-1, 0, NULL, 0);

		// strLen > 0, strLen < 0: 오류 상황
		if (strLen < 0) {
			break;
		}
		buf[strLen] = 0; // 스트링의 끝 표시
		printf("%s", buf);
	}

	closesocket(hRecvSock);
	WSACleanup();
	return 0;
}

void ErrorHandling(char *message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}