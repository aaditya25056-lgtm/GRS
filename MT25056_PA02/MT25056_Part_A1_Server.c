/*
 * Roll No: MT25056
 * Part A1 - Server
 * Two-Copy TCP Implementation using send()
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define BACKLOG 10
#define FIELDS 8


/* Structure for client thread data */
typedef struct {
    int client_fd;
    int field_size;
    char *field[FIELDS];
} client_data_t;


/* Thread function: handle one client */
void* handle_client(void *arg)
{
    client_data_t *data = (client_data_t*)arg;

    int fd = data->client_fd;
    int size = data->field_size;

    char *ack = (char*)malloc(size);

    while (1)
    {
        /* Fill message fields */
        for (int i = 0; i < FIELDS; i++)
        {
            memset(data->field[i], 'A' + i, size);
        }

        /* Send all fields */
        for (int i = 0; i < FIELDS; i++)
        {
            ssize_t sent = send(fd, data->field[i], size, 0);

            if (sent <= 0)
            {
                perror("send failed");
                close(fd);
                pthread_exit(NULL);
            }
        }

        /* Receive acknowledgement */
        ssize_t recvd = recv(fd, ack, size, 0);

        if (recvd <= 0)
        {
            perror("recv failed");
            close(fd);
            pthread_exit(NULL);
        }
    }

    free(ack);
    close(fd);
    pthread_exit(NULL);
}


int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <port> <field_size>\n", argv[0]);
        exit(1);
    }

    int port = atoi(argv[1]);
    int field_size = atoi(argv[2]);

    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    /* Create socket */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0)
    {
        perror("socket");
        exit(1);
    }

    /* Server address */
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    /* Bind */
    if (bind(server_fd,
            (struct sockaddr*)&server_addr,
            sizeof(server_addr)) < 0)
    {
        perror("bind");
        exit(1);
    }

    /* Listen */
    listen(server_fd, BACKLOG);

    printf("Server listening on port %d\n", port);

    /* Accept clients */
    while (1)
    {
        client_fd = accept(server_fd,
                          (struct sockaddr*)&client_addr,
                          &addr_len);

        if (client_fd < 0)
        {
            perror("accept");
            continue;
        }

        /* Allocate client data */
        client_data_t *data =
            (client_data_t*)malloc(sizeof(client_data_t));

        data->client_fd = client_fd;
        data->field_size = field_size;

        /* Allocate 8 dynamic fields */
        for (int i = 0; i < FIELDS; i++)
        {
            data->field[i] = (char*)malloc(field_size);
        }

        pthread_t tid;

        /* Create client thread */
        pthread_create(&tid, NULL,
                       handle_client, data);

        pthread_detach(tid);
    }

    close(server_fd);
    return 0;
}
