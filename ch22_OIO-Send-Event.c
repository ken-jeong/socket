// gcc ch22_OIO-Send-Event.c -o ch22_OIO-Send-Event.exe -lws2_32

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")

void error_handling(char *msg);

int main(void) {
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        error_handling("WSAStartup() error!");
    }

    SOCKET send_socket = WSASocket( /* Overlapped IO 소켓 생성 */
        PF_INET, SOCK_STREAM, 0,
        NULL, 0,
        WSA_FLAG_OVERLAPPED
    );
    if (send_socket == INVALID_SOCKET) { error_handling("WSASocket() error!"); }

    SOCKADDR_IN send_addr;
    memset(&send_addr, 0, sizeof(send_addr));
    send_addr.sin_family      = AF_INET;
    send_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    send_addr.sin_port        = htons(9000);

    printf("Client> Connecting...\n");
    int result = connect(
        send_socket,
        (SOCKADDR*)&send_addr, sizeof(send_addr)
    );
    if (result == SOCKET_ERROR) { error_handling("connect() error!"); }

    /*
        2. 이벤트 생성, 등록 및 버퍼 구조체 설정하기
    */
    WSAEVENT wsa_event = WSACreateEvent();
    if (wsa_event == WSA_INVALID_EVENT) { error_handling("WSACreateEvent() error!"); }

    WSAOVERLAPPED overlapped;
    memset(&overlapped, 0, sizeof(overlapped));
    overlapped.hEvent = wsa_event;

    WSABUF wsa_buf;
    char msg[] = "Network is Computer!";
    wsa_buf.len = sizeof(msg);
    wsa_buf.buf = msg;

    printf("Client> Attempting to send data after sleeping for 10 seconds...\n");
    Sleep(10000);

    printf("Client> Sending data...\n");
    DWORD send_bytes = 0;
    result = WSASend( /* Overlapped IO Send */
        send_socket, &wsa_buf, 1, &send_bytes, 0,
        &overlapped, NULL
    );

    if (result == SOCKET_ERROR) {
        if (WSAGetLastError() == WSA_IO_PENDING) {
            printf("Client> Background data transmission in progress.\n");
            WSAWaitForMultipleEvents( /* 전송 완료 이벤트 발생 대기하기 */
                1, &wsa_event,
                TRUE, WSA_INFINITE, FALSE
            );
            WSAGetOverlappedResult( /* Overlapped IO 이벤트 결과 확인하기 (송신 데이터 크기 확인) */
                send_socket, &overlapped, &send_bytes,
                FALSE, NULL
            );
        } else {
            error_handling("WSASend() error");
        }
    }
    printf("Client> Data sending complete. Size: %lu bytes\n", send_bytes);

    WSACloseEvent(wsa_event);
    closesocket(send_socket);
    WSACleanup();
    return 0;
}

void error_handling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}
