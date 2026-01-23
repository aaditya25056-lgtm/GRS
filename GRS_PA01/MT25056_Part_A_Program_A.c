#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "MT25056_Part_B_workers.h"

// --- FINAL CALIBRATED COUNTS (Target: ~3-5 seconds) ---
#define CPU_LOOP 2100000000UL // 2.1 Billion (~3-5 sec)
#define MEM_LOOP 500          // 500 Passes (~3 sec)
#define IO_LOOP  10000000     // 10 Million (~3 sec)

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3) {
        printf("Usage: %s <cpu|mem|io> [num_processes]\n", argv[0]);
        return 1;
    }

    int num_processes = 1;
    if (argc == 3) {
        num_processes = atoi(argv[2]);
        if (num_processes < 1) num_processes = 1;
    }

    // Use unsigned long for CPU to avoid overflow warnings if needed
    unsigned long count = CPU_LOOP; 
    if (strcmp(argv[1], "mem") == 0) count = MEM_LOOP;
    else if (strcmp(argv[1], "io") == 0) count = IO_LOOP;

    for (int i = 0; i < num_processes; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            if (strcmp(argv[1], "cpu") == 0) cpu_task((int)count);
            else if (strcmp(argv[1], "mem") == 0) mem_task((int)count);
            else if (strcmp(argv[1], "io") == 0) io_task((int)count);
            exit(0);
        }
    }

    for (int i = 0; i < num_processes; i++) wait(NULL);
    return 0;
}