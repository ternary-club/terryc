#ifndef STD_MATH_H
#define STD_MATH_H
#include "math.h"
#endif

#ifndef STD_INT_H
#define STD_INT_H
#include "int.h"
#endif

// Macros for two bit notation
#define b00 0
#define b01 1
#define b10 2
#define b11 3

// 1 char = 8 bits
#define CHAR_BIT 8U

// 1 trit = 2 bits
#define TRIT_BIT CEIL(3U, 2U)

// 1 tryte = 9 trits
#define TRYTE_TRIT 9U

// 1 word = 3 trytes
#define WORD_TRYTE 3U

// 1 word = 27 trits
#define WORD_TRIT (WORD_TRYTE * TRYTE_TRIT)

// 1 byte = 4 trits
#define BYTE_TRIT (CHAR_BIT / TRIT_BIT)

// 1 tryte = 3 bytes
#define TRYTE_BYTE (CEIL(TRYTE_TRIT, BYTE_TRIT))

// 1 word = 7 bytes
#define WORD_BYTE (CEIL(WORD_TRIT, BYTE_TRIT))

// 1 word = 7 bytes
#define TRIPLE_WORD_BYTE (CEIL(WORD_TRIT * 3U, BYTE_TRIT))

// Heptavintimal = 27
#define HEPTA_VINTIMAL 27U

// 1 heptavintimal character = 3 trits
#define HEPTA_VINTIMAL_TRIT (HEPTA_VINTIMAL / TRYTE_TRIT)

// Max value of a tryte = 3^9 - 1 = 19682
#define TRYTE_MAX 19682L

// Max value of a balanced tryte = 3^9 / 2 = 9841
#define BTRYTE_MAX 9841L

// Min value of a balanced tryte = 3^9 / 2 * -1 = -9841
#define BTRYTE_MIN -9841L

// Max value of a tryte = 3^9 - 1 = 19682
#define WORD_MAX 7625597484987ll

// Amount of bytes for storing 3^81 - 1 = 443426488243037769948249630619149892802 (136/8 = 17 bytes)
#define TRIPLE_BYTES 17U

// 1 Kitri (Kt) = 3^7 = 2187
#define KITRI 2187U

// 1 Metri (Mt) = 3^7^2 = 2187^2 = 4782969
#define METRI 4782969U

// 1 Gitri (Gt) = 3^7^3 = 2187^3 = 10460353203
#define GITRI 10460353203ULL

// 1 Tetri (Tt) = 3^7^4 = 2187^4 = 22876792454961
#define TETRI 22876792454961ULL

// Triple Word macro declaration
#define __triple_word(name) static uint8_t name[WORD_BYTE * 3]

// Word macro declaration
#define __word(name) static uint8_t name[WORD_BYTE]

// Tryte macro declaration
#define __tryte(name) static uint8_t name[TRYTE_BYTE]

// Trybble macro declaration
#define __trybble(name) static uint8_t name

// Trit macro declaration
#define __trit(name) uint8_t name

// Trit macro declaration
#define __trit_ret uint8_t

// Tryte buffer macro declaration
#define __tryte_buffer(name, count) static uint8_t name[CEIL(count * TRYTE_TRIT, BYTE_TRIT)]

// Tryte pointer without name (return type) macro declaration
#define __tryte_ret uint8_t*

// Tryte pointer macro declaration
#define __tryte_ptr(name) uint8_t *name

// Byte a trit is in
#define __byte_of_trit(t) ((t)*TRIT_BIT / CHAR_BIT)

// Offset of 0b11 inside a byte
#define __trit_offset(i) ((BYTE_TRIT - 1 - (i) % BYTE_TRIT) * TRIT_BIT)

// Get array form of filling a tryte
#define __fill_tryte(t, v0, v1, v2) t[0] = v0; \
                                    t[1] = v1; \
                                    t[2] = v2

// A 9841 valued balanced tryte definition
#define __tryteMAX {170U, 170U, 128U}
// A 1 valued balanced tryte definition
#define __tryteONE {85U, 85U, 128U}
// A 0 valued balanced tryte definition
#define __tryteZERO {85U, 85U, 64U}
// A -9840 valued balanced tryte definition
#define __tryteMINP1 {0U, 0U, 64U}
// A -9841 valued balanced tryte definition
#define __tryteMIN {0U, 0U, 0U}