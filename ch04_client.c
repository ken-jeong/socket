// 클라이언트 프로그램
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")

#define BUF_SIZE 1000
void ErrorHandling(char* message);

int main()
{
	WSADATA			wsaData;
	SOCKET			hSocket;
	char			message[BUF_SIZE];
	int				strLen;
	SOCKADDR_IN		servAdr;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	// 1. client 소켓 생성하기...
	hSocket = socket(PF_INET, SOCK_STREAM, 0); // TCP
	if (hSocket == INVALID_SOCKET) {
		printf("Client> socket() error.\n");
		WSACleanup();
		return 1;
	}

	// 2. 서버와 TCP 연결하기...
	SOCKADDR_IN servAddr; 
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET; // IPv4
	servAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	servAddr.sin_port = htons(9000); 
	int ret;

	ret = connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr));
	if (ret == SOCKET_ERROR) {
		printf("Client> connect() error, code=%d\n", WSAGetLastError());
		closesocket(hSocket);
		WSACleanup();
		return 1;
	}

	// 서버와 TCP 연결 성공...
	printf("Client> 서버와 TCP 연결 성공.\n");
	int flag = 1;
	while (flag) {
		// 1. 사용자로부터 스트링 입력 받기(q 입력 종료)
		printf("Client> 메시지 입력(q:종료): ");
		scanf("%s", message);

		// "q" 입력을 처리하는 부분 추가.
		if (!strcmp(message, "q")) {
			//  사용자가 "q"를 입력한 상태...
			break;
		}
		
		// 2. 입력된 스트링을 서버로 송신. send()
		ret = send(hSocket, message, strlen(message), 0);
		if (ret == SOCKET_ERROR) {
			printf("<ERROR> send() error. code = %d\n", WSAGetLastError());
			break;
		}
		else { // 송신 성공...
			printf("Client> 메시지 송신 성공(%d 바이트).\n", ret);
		}

		// 3. 서버로부터 스트링을 수신하여 출력. recv()
		// TCP recv 작성 방법: 
		// - 조건 수신 데이터 량이 정해진 경우...
		// ---> 정해진 량을 수신하기 위해 recv Loop 돌아야 함.
		
		int rcvTotal = ret; // 수신 목표치
		int rcvSum = 0;		// 수신 누적치, 0 초기값.
		while (rcvSum < rcvTotal) {
			strLen = recv(hSocket, &message[rcvSum], BUF_SIZE - 1, 0);
			if (strLen == 0) { // 상대방이 소켓 종료 socket close
				printf("Client> 서버가 연결 종료 함.\n");
				flag = 0;
				break;
			}
			else { // 정상 수신...
				message[strLen] = 0; // String의 마지막을 의미...
				printf("Client> 서버로부터 데이터 수신(%d bytes).: %s\n",
					strLen, message);
			}
			rcvSum += strLen;
			message[rcvSum] = 0; // 스트링의 마지막 표시...
			printf("Client> message from server: %s (rcvSum:%d, rcvTotal:%d)\n",
				message, rcvSum, rcvTotal
			);
		}	
	}

	closesocket(hSocket);

	WSACleanup();
	return 0;
}

void ErrorHandling(char* message)
{
	printf("[ERROR] %s \n", message);
	exit(1);
}