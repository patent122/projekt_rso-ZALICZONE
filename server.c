#define _GNU_SOURCE

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#include "shared.h"

void process_square_root_scenario(int client_socket, int request_id);
void process_time_scenario(int client_socket, int request_id);

int main(int argc, char const *argv[])
{
    int server_socket, client_socket, request_id;
    unsigned int t;
    struct sockaddr_in server_info, client_info;
    unsigned char buffer[8];
    ssize_t length;
    socklen_t client_socket_length;

    if ((server_socket = TEMP_FAILURE_RETRY(socket(AF_INET, SOCK_STREAM, 0))) == -1)
    {
        perror("Can't create socket\n");
        return -1;
    }

    server_info.sin_family = AF_INET;
    server_info.sin_addr.s_addr = htonl(INADDR_ANY);
    server_info.sin_port = htons(SERVER_PORT);

    if (TEMP_FAILURE_RETRY(bind(server_socket, &server_info, sizeof(server_info))) == -1)
    {
        perror("Can't bind!\n");
        close(server_socket);
        return -2;
    }

    if (TEMP_FAILURE_RETRY(listen(server_socket, SOMAXCONN)))
    {
        perror("Can't listen!\n");
        close(server_socket);
        return -3;
    }

    client_socket_length = sizeof(client_info);
    printf("Listening...\n");
    while ((client_socket = TEMP_FAILURE_RETRY(accept(server_socket, &client_info, &client_socket_length))) != -1)
    {
        while ((length = TEMP_FAILURE_RETRY(recv(client_socket, buffer, 8, 0))) > 0)
        {
            memcpy(&t, buffer, 4);
            memcpy(&request_id, buffer + 4, 4);
            if (t == SQUARE_ROOT_REQUEST_TYPE)
            {
                process_square_root_scenario(client_socket, request_id);
            }
            else if (t == TIME_REQUEST_TYPE)
            {
                process_time_scenario(client_socket, request_id);
            }
            else
            {
                perror("Invalid request type!\n");
            }
        }
    }

    close(server_socket);
    return 0;
}

void process_square_root_scenario(int client_socket, int request_id)
{
    unsigned char buffer[16];
    unsigned int t;
    double value;

    t = SQUARE_ROOT_RESPONSE_TYPE;
    memcpy(buffer, &t, 4);
    t = request_id;
    memcpy(buffer + 4, &t, 4);

    if (TEMP_FAILURE_RETRY(recv(client_socket, &value, 8, 0)) <= 0)
    {
        perror("Can't receive!\n");
        return;
    }

    invert_endianness(&value, 8);
    value = sqrt(value);
    invert_endianness(&value, 8);
    memcpy(buffer + 8, &value, 8);

    if (TEMP_FAILURE_RETRY(send(client_socket, buffer, 16, 0)) == -1)
    {
        perror("Can't send!\n");
        return;
    }
}

void process_time_scenario(int client_socket, int request_id)
{
    unsigned char time_buffer[30], *buffer;
    unsigned int t;
    time_t tt;
    time(&tt);
    struct tm *info = localtime(&tt);
    ssize_t time_length;

    strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", info);
    time_length = strlen(time_buffer);
    buffer = malloc(12 + time_length);
    t = TIME_RESPONSE_TYPE;
    memcpy(buffer, &t, 4);
    t = request_id;
    memcpy(buffer + 4, &t, 4);
    memcpy(buffer + 8, &time_length, 4);
    invert_endianness(buffer + 8, 4);
    memcpy(buffer + 12, time_buffer, time_length);

    if (TEMP_FAILURE_RETRY(send(client_socket, buffer, 12 + time_length, 0)) == -1)
    {
        perror("Can't send!\n");
        free(buffer);
        return;
    }

    free(buffer);
}
