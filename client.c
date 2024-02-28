#define _GNU_SOURCE

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include "shared.h"

void process_square_root_scenario(int client_socket, int request_id, double number);
void process_time_scenario(int client_socket, int request_id);

int main(int argc, char const *argv[])
{
    int client_socket, option, request_id = 0;
    struct sockaddr_in server_info;
    double number;
    int c;

    if ((client_socket = TEMP_FAILURE_RETRY(socket(AF_INET, SOCK_STREAM, 0))) == -1)
    {
        perror("Can't create socket\n");
        return -1;
    }

    server_info.sin_family = AF_INET;
    server_info.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    server_info.sin_port = htons(SERVER_PORT);

    if (TEMP_FAILURE_RETRY(connect(client_socket, &server_info, sizeof(server_info))) == -1)
    {
        perror("Can't connect to server\n");
        close(client_socket);
        return -2;
    }

    while (1)
    {
        printf("1) Make square root request\n");
        printf("2) Make time request\n");
        printf("3) Exit\n");
        printf(">>> ");

        if (scanf("%d", &option) != 1)
        {
            printf("Invalid number format!\n");
            while ((c = getchar()) != '\n' && c != EOF)
            {
            }
            continue;
        }

        if (option == 1)
        {
            printf("number = ");
            if (scanf("%lf", &number) != 1)
            {
                printf("Invalid number format!\n");
                while ((c = getchar()) != '\n' && c != EOF)
                {
                }
                continue;
            }
            process_square_root_scenario(client_socket, request_id++, number);
        }
        else if (option == 2)
        {
            process_time_scenario(client_socket, request_id++);
        }
        else if (option == 3)
        {
            break;
        }
        else
        {
            perror("Invalid option!\n");
        }
    }

    close(client_socket);

    return 0;
}

void process_square_root_scenario(int client_socket, int request_id, double number)
{
    unsigned char buffer[16];
    unsigned int t;
    ssize_t length;

    t = SQUARE_ROOT_REQUEST_TYPE;
    memcpy(buffer, &t, 4);
    memcpy(buffer + 4, &request_id, 4);
    memcpy(buffer + 8, &number, 8);
    invert_endianness(buffer + 8, 8);

    if (TEMP_FAILURE_RETRY(send(client_socket, buffer, 16, 0)) == -1)
    {
        perror("Can't send!\n");
        return;
    }

    if ((length = TEMP_FAILURE_RETRY(recv(client_socket, buffer, 16, 0))) <= 0)
    {
        perror("Can't receive!\n");
        return;
    }

    memcpy(&number, buffer + 8, 8);
    invert_endianness(&number, 8);

    if (isnan(number))
    {
        printf("can't calculate, probably negative number as input\n");
    }
    else
    {
        printf("sqrt(number) = %lf\n", number);
    }
}

void process_time_scenario(int client_socket, int request_id)
{
    unsigned char buffer[12], *time_buffer;
    unsigned int t;
    ssize_t length;

    t = TIME_REQUEST_TYPE;
    memcpy(buffer, &t, 4);
    memcpy(buffer + 4, &request_id, 4);

    if (TEMP_FAILURE_RETRY(send(client_socket, buffer, 8, 0)) == -1)
    {
        perror("Can't send!\n");
        return;
    }

    if ((length = TEMP_FAILURE_RETRY(recv(client_socket, buffer, 12, 0))) <= 0)
    {
        perror("Can't receive time header!\n");
        return;
    }

    invert_endianness(buffer + 8, 4);
    memcpy(&t, buffer + 8, 4);
    time_buffer = malloc(t);

    if ((length = TEMP_FAILURE_RETRY(recv(client_socket, time_buffer, t, 0))) <= 0)
    {
        perror("Can't receive time value!\n");
        return;
    }

    printf("time = ");
    for (int i = 0; i < t; i++)
    {
        printf("%c", time_buffer[i]);
    }
    printf("\n");
    free(time_buffer);
}
