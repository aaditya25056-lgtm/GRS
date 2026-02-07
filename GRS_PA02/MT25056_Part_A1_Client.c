/* * Roll No: MT25056 
 * Part A1: Two-Copy Client with Throughput/Latency Measurement
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>

#define SERVER_IP "10.0.0.1"
#define PORT 8080

typedef struct {
    size_t msg_size;
    int duration;
    long long total_bytes;
    double total_latency_us;
    long long iterations;
} ThreadArgs;

void *client_worker(void *args) {
    ThreadArgs *t_args = (ThreadArgs *)args;
    int sock = 0;
    struct sockaddr_in serv_addr;
    char *buffer = malloc(t_args->msg_size);
    struct timeval start, end;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) return NULL;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        free(buffer);
        close(sock);
        return NULL;
    }

    time_t start_run = time(NULL);
    while (time(NULL) - start_run < t_args->duration) {
        gettimeofday(&start, NULL);
        
        ssize_t valread = recv(sock, buffer, t_args->msg_size, 0);
        if (valread <= 0) break;

        gettimeofday(&end, NULL);

        // Calculate latency for this message in microseconds
        double lat = (end.tv_sec - start.tv_sec) * 1000000.0 + (end.tv_usec - start.tv_usec);
        t_args->total_latency_us += lat;
        t_args->total_bytes += valread;
        t_args->iterations++;
    }

    close(sock);
    free(buffer);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <msg_size> <threads> <duration>\n", argv[0]);
        return 1;
    }

    size_t msg_size = atoi(argv[1]);
    int num_threads = atoi(argv[2]);
    int duration = atoi(argv[3]);

    pthread_t threads[num_threads];
    ThreadArgs t_args[num_threads];
    long long global_bytes = 0;
    double global_latency = 0;
    long long global_iterations = 0;

    for (int i = 0; i < num_threads; i++) {
        t_args[i].msg_size = msg_size;
        t_args[i].duration = duration;
        t_args[i].total_bytes = 0;
        t_args[i].total_latency_us = 0;
        t_args[i].iterations = 0;
        pthread_create(&threads[i], NULL, client_worker, &t_args[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
        global_bytes += t_args[i].total_bytes;
        global_latency += t_args[i].total_latency_us;
        global_iterations += t_args[i].iterations;
    }

    // Throughput Calculation: (Bytes * 8 bits) / (duration * 10^9) = Gbps
    double throughput = (double)(global_bytes * 8) / (duration * 1e9);
    double avg_lat = global_latency / global_iterations;

    printf("Throughput: %.4f Gbps\n", throughput);
    printf("Avg Latency: %.2f us\n", avg_lat);

    return 0;
}