/*
 * Roll No: MT25056
 * Part A3 - Server
 * Zero-Copy Implementation using MSG_ZEROCOPY
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/uio.h>
#include <linux/errqueue.h>

#define BACKLOG 10
#define FIELDS 8


typedef struct {
    int client_fd;
    int field_size;
    char *field[FIELDS];
} client_data_t;


/* Read completion from error queue */
void read_completion(int fd)
{
    struct msghdr msg;
    struct iovec iov;
    char buf[256];

    memset(&msg, 0, sizeof(msg));

    iov.iov_base = buf;
    iov.iov_len = sizeof(buf);

    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    recvmsg(fd, &msg, MSG_ERRQUEUE);
}


/* Client handler */
void* handle_client(void *arg)
{
    client_data_t *data = (client_data_t*)arg;

    int fd = data->client_fd;
    int size = data->field_size;

    char *ack = malloc(size);

    struct iovec iov[FIELDS];
    struct msghdr msg;

    /* Setup iovec */
    for (int i = 0; i < FIELDS; i++)
    {
        iov[i].iov_base = data->field[i];
        iov[i].iov_len = size;
    }

    memset(&msg, 0, sizeof(msg));

    msg.msg_iov = iov;
    msg.msg_iovlen = FIELDS;

    while (1)
    {
        /* Fill buffers */
        for (int i = 0; i < FIELDS; i++)
        {
            memset(data->field[i], 'A' + i, size);
        }

        /* Zero-copy send */
        ssize_t sent = sendmsg(fd, &msg, MSG_ZEROCOPY);

        if (sent <= 0)
        {
            perror("sendmsg zerocopy");
            close(fd);
            pthread_exit(NULL);
        }

        /* Read completion notification */
        read_completion(fd);

        /* Receive ack */
        ssize_t recvd = recv(fd, ack, size, 0);

        if (recvd <= 0)
        {
            perror("recv");
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

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0)
    {
        perror("socket");
        exit(1);
    }

    /* Enable zerocopy */
    int enable = 1;

    setsockopt(server_fd,
               SOL_SOCKET,
               SO_ZEROCOPY,
               &enable,
               sizeof(enable));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_fd,
            (struct sockaddr*)&server_addr,
            sizeof(server_addr)) < 0)
    {
        perror("bind");
        exit(1);
    }

    listen(server_fd, BACKLOG);

    printf("A3 Server listening on port %d\n", port);

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

        client_data_t *data =
            malloc(sizeof(client_data_t));

        data->client_fd = client_fd;
        data->field_size = field_size;

        for (int i = 0; i < FIELDS; i++)
        {
            data->field[i] = malloc(field_size);
        }

        pthread_t tid;

        pthread_create(&tid,
                       NULL,
                       handle_client,
                       data);

        pthread_detach(tid);
    }

    close(server_fd);
    return 0;
}
