#undef  UNICODE
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <vector>

void OneClientServer()
{
    WSADATA wsaData;

    auto return_Value = WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in coming_addr;

    struct sockaddr_in saServer;
    saServer.sin_family = AF_INET;
    inet_pton(AF_INET, "0.0.0.0", &saServer.sin_addr);
    saServer.sin_port = htons(12345);
    bind(listen_sock, (sockaddr*)&saServer, sizeof(sockaddr_in));
    listen(listen_sock, SOMAXCONN);

    for (;;) {
        int size = sizeof(coming_addr);
        SOCKET client_sock = accept(listen_sock, (sockaddr*)&coming_addr, &size);
        if (client_sock == INVALID_SOCKET) {

            printf("accept function failed with error: %ld\n", WSAGetLastError());
            continue;
        }
        else {
            char buffer[36];
            inet_ntop(AF_INET, &coming_addr.sin_addr.s_addr, buffer, 36);
            printf("%s connected on socket:%d \n", buffer, static_cast<int>(client_sock));
        }
        char buffer[1024];
        int result;
        do {
            result = recv(client_sock, buffer, 1024, 0);
            if (result > 0) {
                printf("Received %d bytes from client on socket: %d"
                    " the message: %.*s\n", result - 2, static_cast<int>(client_sock), result, buffer);
            }
            else if (result <= 0) {
                printf("Client disconnected.\n\n\n\n");
                Sleep(1000);
                closesocket(client_sock);
                break;
            }
            else {
                closesocket(client_sock);
                break;
            }
        } while (result > 0);
    }

    closesocket(listen_sock);
    WSACleanup();
}

void HandleClientSingleServer   (SOCKET client_sock) {
    char buffer[1024];
    int result;
    do {
        result = recv(client_sock, buffer, 1024, 0);
        if (result > 0) {
            printf("Received %d bytes from client on socket: %d"
                ", the message: %.*s\n", result, static_cast<int>(client_sock), result, buffer);
        }
        else if (result == 0) {
            printf("Client on socket %d disconnected.\n", static_cast<int>(client_sock));
            closesocket(client_sock);
            break;
        }
        else {
            printf("recv function failed with error: %d\n", WSAGetLastError());
            closesocket(client_sock);
            break;
        }
    } while (result > 0);
}

void MultiClientServer() {
    WSADATA wsaData;

    auto return_Value = WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in coming_addr;

    struct sockaddr_in saServer;
    saServer.sin_family = AF_INET;
    inet_pton(AF_INET, "0.0.0.0", &saServer.sin_addr);
    saServer.sin_port = htons(12345);
    bind(listen_sock, (sockaddr*)&saServer, sizeof(sockaddr_in));
    listen(listen_sock, SOMAXCONN);

    std::vector<std::thread> client_threads;

    for (;;) {
        int size = sizeof(coming_addr);
        SOCKET client_sock = accept(listen_sock, (sockaddr*)&coming_addr, &size);
        if (client_sock == INVALID_SOCKET) {
            printf("accept function failed with error: %ld\n", WSAGetLastError());
            continue;
        }
        else {
            char buffer[36];
            inet_ntop(AF_INET, &coming_addr.sin_addr.s_addr, buffer, 36);
            printf("%s connected on socket: %d\n", buffer, static_cast<int>(client_sock));
        }

        client_threads.emplace_back(HandleClientSingleServer, client_sock);
    }

    for (auto& thread : client_threads) {
        thread.join();
    }

    closesocket(listen_sock);
    WSACleanup();
}


void HandleClientMultiServerSingleClient(SOCKET client_sock) {
    char buffer[1024];
    int result;
    do {
        result = recv(client_sock, buffer, 1024, 0);
        if (result > 0) {
            printf("Received %d bytes from client on socket: %d, the message: %.*s\n", result, static_cast<int>(client_sock), result, buffer);
        }
        else if (result == 0) {
            printf("Client on socket %d disconnected.\n", static_cast<int>(client_sock));
            closesocket(client_sock);
            break;
        }
        else {
            printf("recv function failed with error: %d\n", WSAGetLastError());
            closesocket(client_sock);
            break;
        }
    } while (result > 0);
}

void MultiServerSingleClient() {
    WSADATA wsaData;

    auto return_Value = WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in coming_addr;

    struct sockaddr_in saServer;
    saServer.sin_family = AF_INET;
    inet_pton(AF_INET, "0.0.0.0", &saServer.sin_addr);
    saServer.sin_port = htons(12345);
    bind(listen_sock, (sockaddr*)&saServer, sizeof(sockaddr_in));
    listen(listen_sock, SOMAXCONN);

    std::vector<std::thread> client_threads;

    for (;;) {
        int size = sizeof(coming_addr);
        SOCKET client_sock = accept(listen_sock, (sockaddr*)&coming_addr, &size);
        if (client_sock == INVALID_SOCKET) {
            printf("accept function failed with error: %ld\n", WSAGetLastError());
            continue;
        }
        else {
            char buffer[36];
            inet_ntop(AF_INET, &coming_addr.sin_addr.s_addr, buffer, 36);
            printf("%s connected!\n", buffer);
        }

        client_threads.emplace_back(HandleClientMultiServerSingleClient, client_sock);
    }

    for (auto& thread : client_threads) {
        thread.join();
    }

    closesocket(listen_sock);
    WSACleanup();
}

int main() 
{
    MultiClientServer();
    return 0;
}
