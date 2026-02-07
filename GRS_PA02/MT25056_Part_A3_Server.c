/* * Roll No: MT25056 
 * Part A3: Zero-Copy Implementation (MSG_ZEROCOPY)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include "MT25056_Part_A_Common.h"

#ifndef MSG_ZEROCOPY
#define MSG_ZEROCOPY 0x4000000
#endif

#define PORT 8082

void *handle_client_a3(void *socket_desc) {
    int client_sock = *(int*)socket_desc;
    free(socket_desc);

    size_t field_sz = 1024;
    Message msg;
    allocate_message(&msg, field_sz);

    // 1. Enable Zero-Copy on this specific client socket
    int one = 1;
    if (setsockopt(client_sock, SOL_SOCKET, SO_ZEROCOPY, &one, sizeof(one)) < 0) {
        perror("setsockopt SO_ZEROCOPY failed (Note: needs Linux 4.14+)");
    }

    struct iovec iov[8];
    for (int i = 0; i < 8; i++) {
        iov[i].iov_base = msg.fields[i];
        iov[i].iov_len = field_sz;
    }

    struct msghdr out_msg = {0};
    out_msg.msg_iov = iov;
    out_msg.msg_iovlen = 8;

    printf("[Server A3] Zero-Copy established! Streaming on port %d...\n", PORT);

    // 2. Use MSG_ZEROCOPY flag in sendmsg
    while (sendmsg(client_sock, &out_msg, MSG_ZEROCOPY) > 0);

    printf("[Server A3] Client disconnected.\n");
    free_message(&msg);
    close(client_sock);
    return NULL;
}

int main() {
    signal(SIGPIPE, SIG_IGN); // Ignore SIGPIPE to avoid exit code 141
    int server_fd, *new_sock;
    struct sockaddr_in server_addr;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_fd, 10);
    printf("[Server A3] Zero-Copy Server listening on port %d...\n", PORT);

    while ((new_sock = malloc(sizeof(int))) && 
           (*new_sock = accept(server_fd, NULL, NULL))) {
        pthread_t tid;
        pthread_create(&tid, NULL, handle_client_a3, new_sock);
        pthread_detach(tid);
    }
    return 0;
}