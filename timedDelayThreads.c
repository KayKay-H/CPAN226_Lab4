/*
Author: Kayle Hunyinbo
Course: CPAN226
Lab: Lab 4 - Multithreading and Network Concurrency
Description:
This server demonstrates multithreading using pthreads.
Each client connection is handled in a separate thread,
allowing multiple clients to be served concurrently.
*/

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include <pthread.h>

#define PORT 8080

void* handle_client(void* arg)
{
    SOCKET client_socket = *(SOCKET*)arg;
    free(arg);

    printf("Client connected. Processing request...\n");

    // simulate work
    Sleep(5000);

    char message[] = "Request processed\n";
    send(client_socket, message, sizeof(message), 0);

    closesocket(client_socket);

    printf("Client request completed.\n");

    return NULL;
}

int main()
{
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server, client;
    int c;

    printf("Initializing Winsock...\n");

    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0)
    {
        printf("Failed. Error Code : %d", WSAGetLastError());
        return 1;
    }

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket == INVALID_SOCKET)
    {
        printf("Could not create socket : %d", WSAGetLastError());
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server , sizeof(server)) == SOCKET_ERROR)
    {
        printf("Bind failed with error code : %d" , WSAGetLastError());
        return 1;
    }

    listen(server_socket , 5);

    printf("Server listening on port %d...\n", PORT);

    c = sizeof(struct sockaddr_in);

    while((client_socket = accept(server_socket, (struct sockaddr *)&client, &c)))
    {
        pthread_t thread;

        SOCKET* new_sock = malloc(sizeof(SOCKET));
        *new_sock = client_socket;

        if(pthread_create(&thread, NULL, handle_client, (void*)new_sock) < 0)
        {
            perror("Could not create thread");
            return 1;
        }

        pthread_detach(thread);
    }

    closesocket(server_socket);
    WSACleanup();

    return 0;
}
