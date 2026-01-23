#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "MT25056_Part_B_workers.h"

// --- FINAL CALIBRATED COUNTS ---
#define CPU_LOOP 2100000000UL 
#define MEM_LOOP 500       
#define IO_LOOP  10000000    

typedef struct { char *type; int count; } thread_arg_t;

void* worker(void *arg) {
    thread_arg_t *args = (thread_arg_t *)arg;
    if (strcmp(args->type, "cpu") == 0) cpu_task(args->count);
    else if (strcmp(args->type, "mem") == 0) mem_task(args->count);
    else if (strcmp(args->type, "io") == 0) io_task(args->count);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3) {
        printf("Usage: %s <cpu|mem|io> [num_threads]\n", argv[0]);
        return 1;
    }

    int num_threads = 1;
    if (argc == 3) {
        num_threads = atoi(argv[2]);
        if (num_threads < 1) num_threads = 1;
    }

    unsigned long count = CPU_LOOP;
    if (strcmp(argv[1], "mem") == 0) count = MEM_LOOP;
    else if (strcmp(argv[1], "io") == 0) count = IO_LOOP;

    pthread_t threads[num_threads];
    thread_arg_t t_args;
    t_args.type = argv[1];
    t_args.count = (int)count;

    for (int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, worker, &t_args);
    }
    for (int i = 0; i < num_threads; i++) pthread_join(threads[i], NULL);

    return 0;
}