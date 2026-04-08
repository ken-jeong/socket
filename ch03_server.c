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
	WSADATA   winsockData;
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
	svrAddr.sin_addr.s_addr = inet_addr("1.2.3.4");

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





#if 0
	// 1. inet_ntoa()함수 사용 방법
	char* str_ptr;
	str_ptr = inet_ntoa(svrAddr.sin_addr); 
	printf("Addr str = %s\n", str_ptr);

	// 2. WSAStringToAddress() 함수 사용법
	//    "1.2.3.4:9000" -> IP와 port를 모두 스트링으로 표현
	char* addrStr = "5.6.7.8:9000";
	SOCKADDR_IN  addr2;
	int addrSize, ret;
	addrSize = sizeof(addr2);
	ret = WSAStringToAddressA(
			addrStr, // 주소 스트링(IP, Port)
			AF_INET, // IPv4 주소
			NULL,
			(SOCKADDR*)&addr2, // 주소를 담을 주소 구조체 변수
			&addrSize
		);

	if (ret == 0) {
		printf("IP: %#x, Port: %#x \n",
			ntohl(addr2.sin_addr.S_un.S_addr), // Big end
			ntohs(addr2.sin_port)	// Big end
			);
	}
	// 주소 구조체(addr2) -> String "5.6.7.8:9000" 출력하기
	char addrBuf[50];
	int bufSize;
	bufSize = sizeof(addrBuf);
	ret = WSAAddressToStringA(
			(SOCKADDR*)&addr2, // SOCKADDR_IN  SOCKADDR
			sizeof(addr2),
			NULL,
			addrBuf,
			&bufSize
		);
	if (ret == 0) {
		printf("주소 string = %s\n", addrBuf);
	}

	// 내 컴퓨터 CPU의 바이트 저장 순서는? (Big/Little endian)
	unsigned long host_addr = 0x12345678; // ?
	unsigned long net_addr = htonl(host_addr); // Big endian 저장.
	printf("> host 바이트 순서: %#x\n", host_addr);
	printf("> net 바이트 순서(Big end): %#x\n", net_addr);
#endif




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