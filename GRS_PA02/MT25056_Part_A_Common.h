/* Roll No: MT25056 */
#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *fields[8]; 
} Message;

// Helper to allocate and free the message fields
void allocate_message(Message *m, size_t field_size) {
    for(int i = 0; i < 8; i++) {
        m->fields[i] = (char *)malloc(field_size);
        memset(m->fields[i], 'A', field_size); // Fill with dummy data
    }
}

void free_message(Message *m) {
    for(int i = 0; i < 8; i++) {
        free(m->fields[i]);
    }
}

#endif