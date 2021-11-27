#ifndef DRIVER_SYS_H
#define DRIVER_SYS_H
#include "sys.h"
#endif

// Return program break address
void *heap() { return (void *)brk(0); }

// Allocate memory dynamically
void *alloc(uint64_t bytes) {
    intptr h = (uint64_t)heap();
    if (brk(h + bytes) > 0) return (void *)h;
    return NULL;
}

// Free memory dynamically
void free(uint64_t bytes) { brk((uint64_t)heap() - bytes); }
