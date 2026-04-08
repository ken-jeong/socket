// gcc midterm/ch08_gethostbyaddr.c -o midterm/ch08_gethostbyaddr.exe -lws2_32

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")

void ErrorHandling(char* message);

int main() {
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        ErrorHandling("WSAStartup() error!");
    }

    // IP 주소 입력
    char ipAddr[50];
    printf("> Enter IP address: ");
    scanf("%s", ipAddr);
    printf(">> Input IP address: %s\n", ipAddr);

    // gethostbyaddr() 호출
    SOCKADDR_IN addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_addr.s_addr = inet_addr(ipAddr);

    struct hostent* host;
    host = gethostbyaddr((char*)&addr.sin_addr, 4, AF_INET);
    if (!host) {
        printf("<ERROR> gethostbyaddr() error (code = %d)\n",
            WSAGetLastError());
    }

    // 결과 출력
    printf("> Official name: %s\n", host->h_name);
    for (int i = 0; host->h_aliases[i]; i++) {
        printf("- Aliases %d: %s\n", i + 1, host->h_aliases[i]);
    }
    printf("> Address type: %s\n",
        (host->h_addrtype == AF_INET) ? "AF_INET" : "AF_INET6");
    for (int i = 0; host->h_addr_list[i]; i++) {
        printf("- IP addr %d: %s\n", i + 1,
            inet_ntoa(*((struct in_addr*)host->h_addr_list[i])));
    }

    WSACleanup();
    return 0;
}

void ErrorHandling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
