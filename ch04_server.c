// 서버 프로그램.
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#define  MAX_BUF_SIZE  1000
void ErrorHandling(char* message);
int main(void)
{
	char		message[MAX_BUF_SIZE];
	WSADATA		winsockData;
	printf("> 서버 프로그램 실행.\n");

	// 1. 소켓 초기화...소켓 라이브러리를 연결...
	if (WSAStartup(MAKEWORD(2, 2), &winsockData) != 0)
		ErrorHandling("WSAStartup() error!");

	// 2. 소켓을 생성 하기...
	SOCKET serverSocket;
	serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == INVALID_SOCKET)
		ErrorHandling("TCP socket creation error");

	// 3. bind() 서버 소켓에 주소(IP, Port)를 지정하기...IPv4
	struct sockaddr_in6 addr;
	SOCKADDR_IN  svrAddr;
	memset(&svrAddr, 0, sizeof(svrAddr));
	svrAddr.sin_family = AF_INET; // IPv4
	svrAddr.sin_port = htons(9000); // host 순서 -> net 순서 (short)
	svrAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // loopback 주소

	int ret;
	// 2. 서버 소켓에 주소(IP, Port) 설정
	ret = bind(serverSocket, (SOCKADDR*)&svrAddr, sizeof(svrAddr));
	if (ret == SOCKET_ERROR) {
		printf("<ERROR> bind() error. code=%d\n", WSAGetLastError());
		// 소켓을 통한 통신 종료...
		closesocket(serverSocket);
		// 소켓 사용 종료..
		WSACleanup(); 
		return 1;
	}

	// 만반의 서비스 제공 준비: DB 서버 연결 등...

	// 3. listen: TCP 상태를 LISTEN 상태로 변경하기
	ret = listen(serverSocket, 5);
	if (ret == SOCKET_ERROR) {
		printf("<ERROR> listen() error. code=%d\n", WSAGetLastError());
		// 소켓을 통한 통신 종료...
		closesocket(serverSocket);
		// 소켓 사용 종료..
		WSACleanup();
		return 1;
	}

	// 4. client의 연결 요청을 처리해야 함. -> table 앉침..주문받기
	SOCKET ClientSock;
	SOCKADDR_IN  clientAddr;
	int clientAddrLen;
	clientAddrLen = sizeof(clientAddr);
	int strLen;

	while (1)
	{
		printf("Server> 새로운 client로부터 TCP 연결 요청을 기다리는 중...\n");
		ClientSock = accept(serverSocket, (SOCKADDR*)&clientAddr, &clientAddrLen);
		if (ClientSock == INVALID_SOCKET) {
			printf("Server> accept error.\n");
			closesocket(serverSocket);
			WSACleanup();
			return 1;
		}

		// 새로운 client 연결 요청을 위한 소켓 생성 완료...
		printf("Server> 새로운 client(IP:%s, Port:%d)가 연결되었습니다.\n",
			inet_ntoa(clientAddr.sin_addr),
			ntohs(clientAddr.sin_port)   // Big end --> host order 변환
		);

		while (1)
		{
			// 1. client 요청 메시지 수신. recv()
			strLen = recv(ClientSock, message, MAX_BUF_SIZE-1, 0);
			if (strLen == 0) { // 상대방이 소켓 종료 socket close
				printf("Server> client가 연결 종료 함.\n" );
				break;
			}
			else if (strLen == SOCKET_ERROR) {
				printf("Server> recv() SOCKET_ERROR 발생. code=%d\n", 
					WSAGetLastError());
				break;
			}
			else { // 정상 수신...
				message[strLen] = 0; // String의 마지막을 의미...
				printf("Server> client로부터 데이터 수신(%d bytes).: %s\n",
					strLen, message);
			}

			// 2. 메시지 파싱(parsing)

			// 3. client에게 응답 메시지 송신. send()
			ret = send(ClientSock, message, strLen, 0);
			if (ret == SOCKET_ERROR) {
				printf("<ERROR> send() error. code = %d\n", WSAGetLastError());
				break;
			}
			else { // 송신 성공...
				printf("Server> client로 메시지 송신 성공(%d 바이트).\n", ret);
			}
		}

		closesocket(ClientSock);
	}

	// 소켓을 통한 통신 종료...
	closesocket(serverSocket);

	// 소켓 사용 종료..
	WSACleanup();
	return 0;
}

void ErrorHandling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}