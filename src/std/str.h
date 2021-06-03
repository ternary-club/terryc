#ifndef STD_TRYTE_H
#define STD_TRYTE_H
#include "tryte.h"
#endif

#ifndef STD_MATH_H
#define STD_MATH_H
#include "math.h"
#endif

#ifndef DRIVER_IO_H
#define DRIVER_IO_H
#include "../driver/io.h"
#endif

#ifndef STD_STR_H
#define STD_STR_H
#include "std/str.h"
#endif

// Tryte (3 bytes) integer to ternary string
const char *tryte_to_tstring(__tryte_ptr(t)) {
    static char tryteBuffer[2 + TRYTE_TRIT + 1];
    tryteBuffer[0] = '0';
    tryteBuffer[1] = 't';
    for(uint8_t i = 0; i < TRYTE_TRIT; i++) {
        tryteBuffer[2 + i] = '0' + (t[__byte_of_trit(i)]
            & (b11 << __trit_offset(i)) >> __trit_offset(i));
    }
    tryteBuffer[2 + TRYTE_TRIT] = '\0';
    return tryteBuffer;
}

// Balanced tryte (3 bytes) integer to trinary string
const char *btryte_to_string(__tryte_ptr(t)) {
    static char tryteBuffer[TRYTE_TRIT + 1];
    for(uint8_t i = 0; i < TRYTE_TRIT; i++) {
        uint8_t offset = __trit_offset(i);
        switch((t[__byte_of_trit(i)]
            & (b11 << __trit_offset(i))) >> __trit_offset(i)) {
            case 0:
                tryteBuffer[i] = '<';
                break;
            case 1:
                tryteBuffer[i] = '/';
                break;
            case 2:
                tryteBuffer[i] = '>';
                break;
        }
    }
    tryteBuffer[2 + TRYTE_TRIT] = '\0';
    return tryteBuffer;
}
