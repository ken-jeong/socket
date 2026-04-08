// gcc ch23_StableEchoClnt.c -o ch23_StableEchoClnt.exe -lws2_32

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")

#define BUF_SIZE 1024

// 에러 처리 함수 선언
void ErrorHandling(char *message);

int main() {
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        ErrorHandling("WSAStartup() error!");
    }

    // TCP 소켓 생성
    SOCKET hSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (hSocket == INVALID_SOCKET) {
        ErrorHandling("socket() error");
    }

    // 서버 주소 정보 설정 (IP: 127.0.0.1, Port: 9000)
    SOCKADDR_IN servAdr;
    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servAdr.sin_port = htons(9000);

    // 서버에 연결 요청
    if (connect(hSocket, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR) {
        ErrorHandling("connect() error!");
    } else {
        printf("Client> Connected...........\n");
    }

    char message[BUF_SIZE];
    while (1) { // 메시지 송수신 루프
        fputs("Input message(Q to quit): ", stdout);
        fgets(message, BUF_SIZE, stdin);

        // 'q' 또는 'Q' 입력 시 루프 탈출 및 종료
        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n")) {
            break;
        }

        int strLen = strlen(message);
        // 서버로 메시지 전송
        send(hSocket, message, strLen, 0);

        // 서버로부터 메시지 수신 (전송한 크기만큼 데이터가 올 때까지 반복 수신)
        int readLen = 0;
        while (1) {
            readLen += recv(hSocket, &message[readLen], BUF_SIZE-1, 0);
            if (readLen >= strLen) {
                break;
            }
        }
        message[strLen] = 0;
        printf("Client> Message from server: %s\n", message);
    }

    // 소켓 반환 및 윈속 리소스 해제
    closesocket(hSocket);
    WSACleanup();
    return 0;
}

// 에러 메시지 출력 후 프로그램을 종료하는 함수
void ErrorHandling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
