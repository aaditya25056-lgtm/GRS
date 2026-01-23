#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "MT25056_Part_B_workers.h"

void cpu_task(int count) {
    volatile double result = 0.0;
    for (int i = 0; i < count; i++) {
        result += i * i * 0.0001;
    }
}

void mem_task(int count) {
    // 10MB Fixed Buffer
    int fixed_size = 1024 * 1024 * 10 / sizeof(int); 
    int *array = (int *)malloc(fixed_size * sizeof(int));

    if (array == NULL) {
        perror("Memory allocation failed");
        return;
    }

    volatile int temp = 0;
    for (int k = 0; k < count; k++) {
        for (int i = 0; i < fixed_size; i++) {
            array[i] = i + k;
        }
        for (int i = 0; i < fixed_size; i+=1000) {
            temp = array[i];
        }
    }
    (void)temp; 
    free(array);
}

void io_task(int count) {
    const char *filename = "io_output.txt";
    FILE *f = fopen(filename, "w");
    if (f == NULL) return;

    const char *data = "Writing some data to disk to simulate IO load.\n";
    for (int i = 0; i < count; i++) {
        if (fprintf(f, "%s", data) < 0) break; 
    }
    fclose(f);
}