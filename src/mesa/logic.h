#ifndef STD_TRYTE_H
#define STD_TRYTE_H
#include "../std/tryte.h"
#endif

// Convert byte to tryte
__tryte_ret uint16_to_tryte(uint16_t n) {
    // Reset tryte
    __tryte(t);
    t[0] = 0;
    t[1] = 0;
    t[2] = 0;

    // Convert number
    uint16_t i = 0, l = TRYTE_TRIT - 1;
    while (n) {
        t[__byte_of_trit(l - i)] &= ~(b11 << __trit_offset(l - i));
        t[__byte_of_trit(l - i)] |= (n % 3) << __trit_offset(l - i);
        n /= 3;
        i++;
    }
    return t;
}

// Convert 8 bytes to word
__tryte_ret uint64_to_word(uint64_t n) {
    __word(t);
    uint8_t i = 0;
    while (n) {
        uint8_t offset = __trit_offset(WORD_TRIT - 1 - i);
        t[__byte_of_trit(WORD_TRIT - 1 - i)] &= ~(b11 << offset);
        t[__byte_of_trit(WORD_TRIT - 1 - i)] |= (n % 3) << offset;
        n /= 3;
        i++;
    }
    return t;
}

// NOT gate
// xy | z
// 00 | 2
// 01 | 1
// 11 | X
// 10 | 0
//
// This implementation uses the following equation:
// x' = ~x ^ y; y' = y
__trit_ret __not(__trit(t)) {
    __trit(u);
    const uint8_t notX = ~t & b10;
    const uint8_t y = t & b01;
    u &= ~b11;
    u |= notX ^ y << 1 | y;

    return u;
}

// INC gate
// xy | z
// 00 | 1
// 01 | 2
// 11 | X
// 10 | 0
//
// This implementation uses the following equation:
// x' = y; y' = ~x ^ y
__trit_ret __inc(__trit(t)) {
    __trit(u);
    const uint8_t notX = ~t & b10;
    const uint8_t y = t & b01;
    u &= ~(b11);
    u |= (y << 1 | notX >> 1 ^ y);
    return u;
}

// DEC gate
// xy | z
// 00 | 2
// 01 | 0
// 11 | X
// 10 | 1
//
// This implementation uses the following equation:
// x' = x ^ ~y; y' = x
__trit_ret __dec(__trit(t)) {
    __trit(u);
    const uint8_t x = t & b10;
    const uint8_t notY = ~t & b01;
    u &= ~b11;
    u |= (x ^ notY << 1 | x >> 1);
    return u;
}

// ISF gate
// xy | z
// 00 | 2
// 01 | 0
// 11 | X
// 10 | 0
//
// This implementation uses the following equation:
// x' = ~x ^ y; y' = 0
__trit_ret __isf(__trit(t)) {
    __trit(u);
    const uint8_t notX = ~t & b10;
    const uint8_t y = t & b01;
    u &= ~b11;
    u |= notX ^ y << 1;
    return u;
}

// ISU gate
// xy | z
// 00 | 0
// 01 | 2
// 11 | X
// 10 | 0
//
// This implementation uses the following equation:
// x' = y; y' = 0
__trit_ret __isu(__trit(t)) {
    __trit(u);
    const uint8_t y = t & b01;
    u &= ~b11;
    u |= y << 1;
    return u;
}

// IST gate
// xy | z
// 00 | 0
// 01 | 0
// 11 | X
// 10 | 2
//
// This implementation uses the following equation:
// x' = x; y' = 0
__trit_ret __ist(__trit(t)) {
    __trit(u);
    const uint8_t x = (t & b10);
    u &= ~b11;
    u |= x;
    return u;
}

// CLD gate
// xy | z
// 00 | 0
// 01 | 1
// 11 | X
// 10 | 1
//
// This implementation uses the following equation:
// x' = 0; y' = x ^ y
__trit_ret __cld(__trit(t)) {
    __trit(u);
    const uint8_t x = t & b10;
    const uint8_t y = t & b01;
    u &= ~b11;
    u |= x >> 1 ^ y;
    return u;
}

// CLU gate
// xy | z
// 00 | 1
// 01 | 1
// 11 | X
// 10 | 2
//
// This implementation uses the following equation:
// x' = x; y' = ~x
__trit_ret __clu(__trit(t)) {
    __trit(u);
    const uint8_t x = t & b10;
    const uint8_t notX = ~t & b10;
    u &= ~b11;
    u |= x | notX >> 1;
    return u;
}

// MIN or AND gate
// zw  00 01 11 10
// xy  -----------
// 00 | 0  0  X  0
// 01 | 0  1  X  1
// 11 | X  X  X  X
// 10 | 0  1  X  2
//
// This implementation uses the following equation:
// x' = x & z; y' = ((x ^ y) & (z ^ w) & ~(x & z))
__trit_ret __and(__trit(t), __trit(u)) {
    __trit(v);
    const uint8_t x = t & b10;
    const uint8_t y = t & b01;
    const uint8_t z = u & b10;
    const uint8_t w = u & b01;
    v &= ~b11;
    v |= x & z | ((x >> 1 ^ y) & (z >> 1 ^ w) & ~(x & z) >> 1);
    return v;
}

// MAX or OR gate
// zw  00 01 11 10
// xy  -----------
// 00 | 0  1  X  2
// 01 | 1  1  X  2
// 11 | X  X  X  X
// 10 | 2  2  x  2
//
// This implementation uses the following equation:
// x' = x | z; y' = (y | w) & ~(z ^ x)
__trit_ret __or(__trit(t), __trit(u)) {
    __trit(v);
    const uint8_t x = t & b10;
    const uint8_t y = t & b01;
    const uint8_t z = u & b10;
    const uint8_t w = u & b01;
    v &= ~b11;
    v |= x | z | ((y | w) & ~(z >> 1 ^ x >> 1));
    return v;
}

// NMIN or NAND
// zw  00 01 11 10
// xy  -----------
// 00 | 2  2  X  2
// 01 | 2  1  X  1
// 11 | X  X  X  X
// 10 | 2  1  X  0
//
// This implementation uses the following equation:
// x' = ~y' & ~(x & z); y' = y & (z | w) | x & w
__trit_ret __nand(__trit(t), __trit(u)) {
    __trit(v);
    const uint8_t x = t & b10;
    const uint8_t y = t & b01;
    const uint8_t z = u & b10;
    const uint8_t w = u & b01;
    const uint8_t yL = (y & (z >> 1 | w) | x >> 1 & w) & b01;
    const uint8_t notYL = ~yL & b01;
    v &= ~b11;
    v |= notYL << 1 & ~(x & z) | yL;
    return v;
}

// NMAX or NOR
// zw  00 01 11 10
// xy  -----------
// 00 | 2  1  X  0
// 01 | 1  1  X  0
// 11 | X  X  X  X
// 10 | 0  0  X  0
//
// This implementation uses the following equation:
// x' = ~(x | y | z | w); y' = ~(x ^ z) & (y | w)
__trit_ret __nor(__trit(t), __trit(u)) {
    __trit(v);
    const uint8_t x = t & b10;
    const uint8_t y = t & b01;
    const uint8_t z = u & b10;
    const uint8_t w = u & b01;
    v &= ~b11;
    v |= ~(x | y << 1 | z | w << 1) & b10 | ~(x ^ z) >> 1 & (y | w);
    return v;
}

// XOR
// zw  00 01 11 10
// xy  -----------
// 00 | 0  1  X  2
// 01 | 1  1  X  1
// 11 | X  X  X  X
// 10 | 2  1  X  0
//
// This implementation uses the following equation:
// x' = x & (~z ^ w) | z & (~x ^ y); y' = w | y
__trit_ret __xor(__trit(t), __trit(u)) {
    __trit(v);
    const uint8_t x = t & b10;
    const uint8_t y = t & b01;
    const uint8_t z = u & b10;
    const uint8_t w = u & b01;
    v &= ~b11;
    v |= x & (~z ^ w << 1) | z & (~x ^ y << 1) | w | y;
    return v;
}

// SUM
// zw  00 01 11 10
// xy  -----------
// 00 | 2  0  X  1
// 01 | 0  1  X  2
// 11 | X  X  X  X
// 10 | 1  2  X  0
//
// This implementation uses the following equation:
// x' = (~y ^ z) & (~x ^ w); y' = z & ~y & ~x| y & w | ~(z | w) & x
__trit_ret __sum(__trit(t), __trit(u)) {
    __trit(v);
    const uint8_t x = t & b10;
    const uint8_t notX = ~t & b10;
    const uint8_t y = t & b01;
    const uint8_t notY = ~t & b01;
    const uint8_t z = u & b10;
    const uint8_t w = u & b01;
    v &= ~b11;
    v |= (notY << 1 ^ z) & (~x ^ w << 1) | z >> 1 & notY & notX >> 1 | y & w |
         ~(z >> 1 | w) & x >> 1;
    return v;
}

// CONSENSUS
// zw  00 01 11 10
// xy  -----------
// 00 | 0  1  X  1
// 01 | 1  1  X  1
// 11 | X  X  X  X
// 10 | 1  1  X  2
//
// This implementation uses the following equation:
// x' = x & z; y' = (x | y | z | w) & ~(x & z)
__trit_ret __con(__trit(t), __trit(u)) {
    __trit(v);
    const uint8_t x = t & b10;
    const uint8_t y = t & b01;
    const uint8_t z = u & b10;
    const uint8_t w = u & b01;
    v &= ~b11;
    v |= x & z | (x >> 1 | y | z >> 1 | w) & ~(x & z) >> 1;
    return v;
}

// ANY
// zw  00 01 11 10
// xy  -----------
// 00 | 0  0  X  1
// 01 | 0  1  X  2
// 11 | X  X  X  X
// 10 | 1  2  X  2
//
// This implementation uses the following equation:
// x' = y & z | x & (z | w); y' = ~(x | y | w) & z | ~(y | z | w) & x | y & w
__trit_ret __any(__trit(t), __trit(u)) {
    __trit(v);
    const uint8_t x = t & b10;
    const uint8_t y = t & b01;
    const uint8_t z = u & b10;
    const uint8_t w = u & b01;
    v &= ~b11;
    v |=
        y << 1 & z | x & (z | w << 1) |
        (~(x >> 1 | y | w) & z >> 1 | ~(y | z >> 1 | w) & x >> 1 | y & w) & b01;
    return v;
}