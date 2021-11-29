#ifndef DRIVER_SYS_H
#define DRIVER_SYS_H
#include "sys.h"
#endif

#ifndef STD_INT_H
#define STD_INT_H
#include "../std/int.h"
#endif

// Get length of string
uint64_t strlen(char const *str) {
    char const *p;
    for (p = str; *p; ++p)
        ;
    return p - str;
}

// Print string to screen
internal uintptr puts(char const *str) {
    return write(stdout, str, strlen(str));
}