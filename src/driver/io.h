#ifndef DRIVER_SYS_H
#define DRIVER_SYS_H
#include "sys.h"
#endif

#ifndef STD_INT_H
#define STD_INT_H
#include "../std/int.h"
#endif

// 8 Bytes signed integer to string
char *itoa(int64_t value) {
    static char intBuffer[16];
    char tmp[16];
    char *sp = intBuffer;
    char *tp = tmp;
    uint32_t v;

    uint8_t sign = value < 0;    
    if(sign) v = -value;
    else v = value;

    while(v || tp == tmp) {
        uint8_t i = v % 10;
        v /= 10;
        *tp++ = i + '0';
    }

    uint8_t len = tp - tmp;
    if(sign) {
        *tp++ = '-';
        len++;
    }
    while(tp > tmp) *sp++ = *--tp;

    intBuffer[len] = '\0';

    return intBuffer;
}

// Get length of string
uint64_t strlen(char const *str) {
    char const *p;
    for (p = str; *p; ++p);
    return p - str;
}

// Print string to screen
internal
uintptr puts(char const *str) {
    return write(stdout, str, strlen(str));
}