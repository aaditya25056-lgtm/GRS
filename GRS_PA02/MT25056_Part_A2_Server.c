/* Roll No: MT25056 - Part A2 Server */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "MT25056_Part_A_Common.h"
#include <signal.h>



#define PORT 8081 

void *handle_client_a2(void *socket_desc) {
    int client_sock = *(int*)socket_desc;
    free(socket_desc);

    size_t field_sz = 1024; 
    Message msg;
    allocate_message(&msg, field_sz);

    struct iovec iov[8];
    for (int i = 0; i < 8; i++) {
        iov[i].iov_base = msg.fields[i];
        iov[i].iov_len = field_sz;
    }

    struct msghdr out_msg = {0};
    out_msg.msg_iov = iov;
    out_msg.msg_iovlen = 8;

    printf("[Server A2] Connection established! Streaming data...\n");

    while (sendmsg(client_sock, &out_msg, 0) > 0);

    printf("[Server A2] Client disconnected.\n");
    free_message(&msg);
    close(client_sock);
    return NULL;
}

int main() {
    // Inside main():
    signal(SIGPIPE, SIG_IGN);
    int server_fd, *new_sock;
    struct sockaddr_in server_addr;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        return 1;
    }
    listen(server_fd, 10);
    printf("[Server A2] One-Copy Server listening on port %d...\n", PORT);

    while (1) {
        new_sock = malloc(sizeof(int));
        *new_sock = accept(server_fd, NULL, NULL);
        if (*new_sock < 0) {
            free(new_sock);
            continue;
        }
        pthread_t tid;
        pthread_create(&tid, NULL, handle_client_a2, new_sock);
        pthread_detach(tid);
    }
    return 0;
}