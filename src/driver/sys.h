#ifndef STD_INT_H
#define STD_INT_H
#include "../std/int.h"
#endif

#ifndef DRIVER_CALLS_H
#define DRIVER_CALLS_H
#include "calls.h"
#endif

typedef uint64_t uintptr;
typedef int64_t intptr;

#define internal static

// Perform a system call with 1 parameter
void *syscall1(uintptr number, void *arg1);

// Perform a system call with 3 parameters
void *syscall3(uintptr number, void *arg1, void *arg2, void *arg3);

#define stdout 1

// Perform a 'write' system call
internal intptr write(int fd, void const *data, uintptr nbytes) {
    return (intptr)syscall3(SYS_write, (void *)(intptr)fd, (void *)data,
                            (void *)nbytes);
}

// Perform an 'open' system call
internal intptr open(void const *filename) {
    return (intptr)syscall3(SYS_open, (void *)filename, (void *)0, (void *)777);
}

// Perform a 'read' system call
internal intptr read(uint64_t fileDescriptor, char *buffer, uint64_t count) {
    return (intptr)syscall3(SYS_read, (void *)fileDescriptor, (void *)buffer,
                            (void *)count);
}

// Perform a 'close' system call
internal intptr close(uint64_t fileDescriptor) {
    return (intptr)syscall1(SYS_close, (void *)fileDescriptor);
}

// Perform an 'exit' system call
internal intptr exit(uint64_t statusCode) {
    return (intptr)syscall1(SYS_exit, (void *)statusCode);
}

// Perform a 'brk' system call
internal intptr brk(uint64_t address) {
    return (intptr)syscall1(SYS_brk, (void *)address);
}
