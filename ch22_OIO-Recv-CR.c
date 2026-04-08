// gcc ch22_OIO-Recv-CR.c -o ch22_OIO-Recv-CR.exe -lws2_32

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")

#define BUF_SIZE 1024

void CALLBACK completion_routine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void error_handling(char *message);

WSABUF wsa_buf;
char   buf[BUF_SIZE];
DWORD  recv_bytes = 0;

int main() {
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        error_handling("WSAStartup() error!");
    }

    /* listen_socket */
    SOCKET listen_socket = WSASocket( /* Overlapped IO 소켓 생성 */
        AF_INET, SOCK_STREAM, 0,
        NULL, 0,
        WSA_FLAG_OVERLAPPED
    );
    if (listen_socket == INVALID_SOCKET) { error_handling("WSASocket() error"); }

    SOCKADDR_IN listen_addr;
    memset(&listen_addr, 0, sizeof(listen_addr));
    listen_addr.sin_family      = AF_INET;
    listen_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    listen_addr.sin_port        = htons(9000);

    int result = bind(
        listen_socket,
        (SOCKADDR*)&listen_addr, sizeof(listen_addr)
    );
    if (result == SOCKET_ERROR) { error_handling("bind() error"); }

    result = listen(listen_socket, 5);
    if (result == SOCKET_ERROR) { error_handling("listen() error"); }

    /* wsarecv_socket */
    SOCKADDR_IN addr_recv;
    int sizeof_addr_recv = sizeof(addr_recv);

    SOCKET wsarecv_socket = accept(
        listen_socket,
        (SOCKADDR*)&addr_recv, &sizeof_addr_recv
    );
    if (wsarecv_socket == INVALID_SOCKET) { error_handling("accept() error"); }
    printf("Server> accept()\n");

    // 2. Overlapped 구조체 초기화 및 버퍼 구조체 설정하기
    WSAOVERLAPPED overlapped;
    memset(&overlapped, 0, sizeof(overlapped));
    wsa_buf.len = BUF_SIZE;
    wsa_buf.buf = buf;

    // 3. 이벤트 생성하기 (더미 이벤트 객체)
    WSAEVENT ev_obj = WSACreateEvent();
    if (ev_obj == WSA_INVALID_EVENT_PARAMETER) { error_handling("WSACreateEvent() error"); }

    printf("Server> recv().\n");
    DWORD flags = 0;
    result = WSARecv( /* Overlapped IO Recv */
        wsarecv_socket, &wsa_buf, 1, &recv_bytes, &flags,
        &overlapped, completion_routine
    );

    if (result == SOCKET_ERROR) {
        if (WSAGetLastError() == WSA_IO_PENDING) {
            printf("Server> Background data receive...\n");
        }
    }

    // 4. 이벤트 발생 대기하기 (Alertable Wait 상태 진입)
    printf("Server> enter alertable wait.\n");
    DWORD idx = WSAWaitForMultipleEvents(
        1, &ev_obj,
        FALSE, WSA_INFINITE, TRUE
    );

    if (idx == WAIT_IO_COMPLETION) {
        printf("Server> Overlapped I/O Completed...\n");
    } else { // 에러가 발생한 경우
        error_handling("WSARecv() error");
    }

    WSACloseEvent(ev_obj);
    closesocket(wsarecv_socket);
    closesocket(listen_socket);
    WSACleanup();
    return 0;
}

void CALLBACK completion_routine(DWORD dw_error, DWORD sz_recv_bytes, LPWSAOVERLAPPED lp_overlapped, DWORD flags) {
    if (dw_error != 0) {
        error_handling("Comp> completion_routine error");
    } else {
        recv_bytes = sz_recv_bytes;
        printf("Comp> Received message: %s \n", buf);
    }
}

void error_handling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
