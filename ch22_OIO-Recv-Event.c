// gcc ch22_OIO-Recv-Event.c -o ch22_OIO-Recv-Event.exe -lws2_32

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")

#define BUF_SIZE 1024

void error_handling(char *message);

int main(void) {
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        error_handling("WSAStartup() error");
    }

    SOCKET listen_socket = WSASocket( /* Overlapped IO 소켓 생성 */
        PF_INET, SOCK_STREAM, 0,
        NULL, 0,
        WSA_FLAG_OVERLAPPED
    );
    if (listen_socket == INVALID_SOCKET) { error_handling("WSASocket() error"); }

    SOCKADDR_IN listen_addr;
    memset(&listen_addr, 0, sizeof(listen_addr));
    listen_addr.sin_family        = AF_INET;
    listen_addr.sin_addr.s_addr    = inet_addr("127.0.0.1");
    listen_addr.sin_port        = htons(9000);

    int result = bind(
        listen_socket,
        (SOCKADDR*)&listen_addr, sizeof(listen_addr)
    );
    if (result == SOCKET_ERROR) { error_handling("bind() error"); }

    result = listen(listen_socket, 5);
    if (result == SOCKET_ERROR) { error_handling("listen() error"); }

    SOCKADDR_IN recv_addr;
    int sizeof_recv_addr = sizeof(recv_addr);

    SOCKET recv_socket = accept(
        listen_socket,
        (SOCKADDR*)&recv_addr, &sizeof_recv_addr
    );

    if (recv_socket == INVALID_SOCKET) { error_handling("accept() error"); }
    printf("Server> accept() completed.\n");

    // --- 시작
    // 2. 이벤트 생성 & 등록, 버퍼 구조체 설정하기
    WSAEVENT wsa_event = WSACreateEvent();
    if (wsa_event == WSA_INVALID_EVENT) { error_handling("WSACreateEvent() error"); }

    WSAOVERLAPPED overlapped;
    memset(&overlapped, 0, sizeof(overlapped));
    overlapped.hEvent = wsa_event;

    char buf[BUF_SIZE];
    WSABUF wsa_buf;
    wsa_buf.len = BUF_SIZE;
    wsa_buf.buf = buf;

    printf("Server> Calling WSARecv().\n");
    DWORD recv_bytes = 0, flags = 0;
    result = WSARecv( /* Overlapped IO Recv */
        recv_socket, &wsa_buf, 1, &recv_bytes, &flags,
        &overlapped, NULL
    );

    if (result == SOCKET_ERROR) {
        if (WSAGetLastError() == WSA_IO_PENDING) {
            printf("Server> Background data receiving...\n");
            WSAWaitForMultipleEvents( /* 이벤트 발생 대기하기 */
                1, &wsa_event,
                TRUE, WSA_INFINITE, FALSE
            );
            WSAGetOverlappedResult( /* 이벤트 결과 확인하기 (수신 데이터 크기 확인) */
                recv_socket, &overlapped, &recv_bytes,
                FALSE, &flags
            );
            printf("Server> Received message: %s\n", buf);
        } else {
            error_handling("WSARecv() error");
        }
    } else { /* 동기적으로 데이터 수신이 바로 완료되었을 때의 처리 */
        printf("Server> Received message immediately: %s\n", buf);
    }

    WSACloseEvent(wsa_event);
    closesocket(recv_socket);
    closesocket(listen_socket);
    WSACleanup();
    return 0;
}

void error_handling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
