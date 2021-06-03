#ifndef STD_INT_H
#define STD_INT_H
#include "int.h"
#endif

// Ceil macro
#define CEIL(x,y) (((x) + (y) - 1) / (y))

// Power
uint64_t power(const uint64_t base, const uint8_t exponent) {
    uint64_t result = 1;
    for(uint8_t i = 0; i < exponent; i++)
        result *= base;
    return result;
}
