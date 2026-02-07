/* * Roll No: MT25056 
 * Part A1: Two-Copy Baseline Server
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <signal.h>  // <--- ADD THIS
#include "MT25056_Part_A_Common.h"

#define PORT 8080

void *handle_client(void *socket_desc) {
    int client_sock = *(int*)socket_desc;
    free(socket_desc);

    // Note: Ideally this should match the client's requested size, 
    // but for now we fix it to prevent logic errors.
    size_t field_sz = 1024; 
    Message msg;
    allocate_message(&msg, field_sz);

    printf("[Server] Handling client connection...\n");

    while (1) {
        for (int i = 0; i < 8; i++) {
            // Check for write errors to exit gracefully
            if (send(client_sock, msg.fields[i], field_sz, 0) <= 0) {
                goto cleanup;
            }
        }
    }

cleanup:
    printf("[Server] Client disconnected.\n");
    free_message(&msg);
    close(client_sock);
    return NULL;
}

int main() {
    signal(SIGPIPE, SIG_IGN); // <--- ADD THIS LINE TO PREVENT CRASHES
    
    int server_fd, *new_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    int opt = 1; // Reuse address to prevent "Bind failed"
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_fd, 10);

    printf("[Server] Listening on port %d...\n", PORT);

    while ((new_sock = malloc(sizeof(int))) && 
           (*new_sock = accept(server_fd, (struct sockaddr *)&client_addr, &addr_size))) {
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handle_client, (void*)new_sock);
        pthread_detach(thread_id); 
    }

    return 0;
}