#ifndef STD_TYPES_H
#define STD_TYPES_H
#include "../std/types.h"
#endif

bool is_char(char a) {
    return a >= 'a' && a <= 'z' || a >= 'A' && a <= 'Z';
}

bool is_number(char a) {
    return a >= '0' && a <= '9';
}

bool is_empty(char a) {
    return a == ' ' || a == '\t';
}

