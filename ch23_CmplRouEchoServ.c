// gcc ch23_CmplRouEchoServ.c -o ch23_CmplRouEchoServ.exe -lws2_32

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")

#define BUF_SIZE 1024

void CALLBACK ReadCompRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void CALLBACK WriteCompRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void ErrorHandling(char *message);

typedef struct {
    SOCKET hClntSock;
    char   buf[BUF_SIZE];
    WSABUF wsaBuf;
} PER_IO_DATA, *LPPER_IO_DATA;

int main(void) {
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        ErrorHandling("WSAStartup() error!");
    }
    
    // 1. Overlapped IO를 지원하는 소켓 생성
    SOCKET hLisnSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (hLisnSock == INVALID_SOCKET)
        ErrorHandling("WSASocket() error");

    // 2. 넌블로킹(Non-blocking) 모드로 소켓 설정 및 accept() 준비
    int mode = 1;
    if (ioctlsocket(hLisnSock, FIONBIO, &mode) == SOCKET_ERROR)
        ErrorHandling("ioctlsocket() error");

    // 3. 서버 주소 설정 및 bind, listen
    SOCKADDR_IN lisnAdr, recvAdr;
    memset(&lisnAdr, 0, sizeof(lisnAdr));
    lisnAdr.sin_family            = AF_INET;
    lisnAdr.sin_addr.s_addr        = htonl(INADDR_ANY);
    lisnAdr.sin_port            = htons(9000);
    if (bind(hLisnSock, (SOCKADDR*) &lisnAdr, sizeof(lisnAdr))==SOCKET_ERROR)
        ErrorHandling("bind() error");
    if (listen(hLisnSock, 5) == SOCKET_ERROR)
        ErrorHandling("listen() error");

    int recvAdrSz = sizeof(recvAdr);
    while (1) {
        // 4. 콜백(Callback) 함수가 호출될 수 있는 Alertable Wait 상태로 진입
        SleepEx(100, TRUE);

        // 5. 비동기(넌블로킹) accept() 수행
        SOCKET hRecvSock = accept(hLisnSock, (SOCKADDR*)&recvAdr,&recvAdrSz);
        if (hRecvSock == INVALID_SOCKET) {
            if (WSAGetLastError() == WSAEWOULDBLOCK) {
                continue;
            } else {
                ErrorHandling("accept() error");
            }
        }
        printf("Client connected...\n");

        // 6. 비동기 수신(WSARecv) 함수 호출
        LPWSAOVERLAPPEDlpOvLp = (LPWSAOVERLAPPED)malloc(sizeof(WSAOVERLAPPED));
        memset(lpOvLp, 0, sizeof(WSAOVERLAPPED));

        LPPER_IO_DATA hbInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
        hbInfo->hClntSock = hRecvSock;
        (hbInfo->wsaBuf).buf = hbInfo->buf;
        (hbInfo->wsaBuf).len = BUF_SIZE;
        lpOvLp->hEvent = (HANDLE)hbInfo; // 구조체 주소를 hEvent 멤버에 저장하여 콜백 함수로 전달

        DWORD recvBytes;
        int flagInfo = 0;
        WSARecv(hRecvSock, &(hbInfo->wsaBuf), 1, &recvBytes, (LPDWORD)&flagInfo, lpOvLp, ReadCompRoutine);
    }
    closesocket(hRecvSock);
    closesocket(hLisnSock);
    WSACleanup();
    return 0;
}

// 수신 완료 콜백 함수
void CALLBACK ReadCompRoutine(DWORD dwError, DWORD szRecvBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags) {
    // 매개변수로 전달된 lpOverlapped 내 hEvent 필드를 통해 사용자 데이터에 접근
    LPPER_IO_DATA hbInfo  = (LPPER_IO_DATA)(lpOverlapped->hEvent);
    SOCKET        hSock   = hbInfo->hClntSock;
    LPWSABUF      bufInfo = &(hbInfo->wsaBuf);

    // 1. 클라이언트가 연결을 종료한 경우 (수신된 바이트가 0인 경우) 처리
    if (szRecvBytes == 0) {
        closesocket(hSock);
        free(lpOverlapped->hEvent); // hbInfo 메모리 해제
        free(lpOverlapped);        // lpOverlapped 메모리 해제
        printf("Client disconnected...\n");
    } else { // 에코(Echo) 송신
        // 2. 데이터 수신이 완료된 경우, 수신된 데이터를 클라이언트에게 다시 전송
        bufInfo->len = szRecvBytes; // 실제 수신한 바이트 수만큼 설정
        DWORD sentBytes;
        WSASend(hSock, bufInfo, 1, &sentBytes, 0, lpOverlapped, WriteCompRoutine);
    }
}

// 송신 완료 콜백 함수
void CALLBACK WriteCompRoutine(DWORD dwError, DWORD szSendBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags) {
    // 1. 클라이언트로의 데이터 전송이 완료된 경우, 새로운 데이터 수신 대기
    LPPER_IO_DATA hbInfo  = (LPPER_IO_DATA)(lpOverlapped->hEvent);
    SOCKET        hSock   = hbInfo->hClntSock;
    LPWSABUF      bufInfo = &(hbInfo->wsaBuf);

    bufInfo->len = BUF_SIZE; // 버퍼 길이를 원래 최대 크기로 복구
    DWORD recvBytes;
    DWORD flagInfo = 0;
    WSARecv(hSock, bufInfo, 1, &recvBytes, &flagInfo, lpOverlapped, ReadCompRoutine);
}

// 에러 처리 함수
void ErrorHandling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
