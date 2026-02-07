/*
 * Roll No: MT25056
 * Part A1 - Client
 * Two-Copy TCP Implementation using recv()
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <time.h>


#define FIELDS 8
#define DURATION 10   // seconds


/* Structure for thread parameters */
typedef struct {
    char server_ip[64];
    int port;
    int field_size;
} client_param_t;


/* Client thread */
void* client_thread(void *arg)
{
    client_param_t *param = (client_param_t*)arg;

    int sockfd;
    struct sockaddr_in server_addr;

    char *buffer = (char*)malloc(param->field_size);

    long long total_bytes = 0;
    long long total_msgs = 0;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
    {
        perror("socket");
        pthread_exit(NULL);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(param->port);

    inet_pton(AF_INET,
              param->server_ip,
              &server_addr.sin_addr);

    if (connect(sockfd,
        (struct sockaddr*)&server_addr,
        sizeof(server_addr)) < 0)
    {
        perror("connect");
        pthread_exit(NULL);
    }

    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);

    while (1)
    {
        clock_gettime(CLOCK_MONOTONIC, &end);

        double elapsed =
            (end.tv_sec - start.tv_sec) +
            (end.tv_nsec - start.tv_nsec) / 1e9;

        if (elapsed >= DURATION)
            break;

        for (int i = 0; i < FIELDS; i++)
        {
            ssize_t recvd =
                recv(sockfd,
                     buffer,
                     param->field_size,
                     0);

            if (recvd <= 0)
            {
                perror("recv");
                close(sockfd);
                pthread_exit(NULL);
            }

            total_bytes += recvd;
        }

        send(sockfd,
             buffer,
             param->field_size,
             0);

        total_msgs++;
    }

    double time_taken =
        (end.tv_sec - start.tv_sec) +
        (end.tv_nsec - start.tv_nsec) / 1e9;

    double throughput =
        (double)total_bytes * 8 /
        (time_taken * 1e9);

    double latency =
        (time_taken / total_msgs) * 1e6;

    printf("Thread %lu: Throughput = %.3f Gbps, Latency = %.3f us\n",
           pthread_self(),
           throughput,
           latency);

    free(buffer);
    close(sockfd);
    pthread_exit(NULL);
}


int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        printf("Usage: %s <server_ip> <port> <field_size> <threads>\n",
               argv[0]);
        exit(1);
    }

    char server_ip[64];
    strcpy(server_ip, argv[1]);

    int port = atoi(argv[2]);
    int field_size = atoi(argv[3]);
    int threads = atoi(argv[4]);

    pthread_t tid[threads];

    client_param_t param;

    strcpy(param.server_ip, server_ip);
    param.port = port;
    param.field_size = field_size;

    /* Create threads */
    for (int i = 0; i < threads; i++)
    {
        pthread_create(&tid[i],
                       NULL,
                       client_thread,
                       &param);
    }

    /* Wait for threads */
    for (int i = 0; i < threads; i++)
    {
        pthread_join(tid[i], NULL);
    }

    return 0;
}
