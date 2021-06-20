#ifndef STD_BOOL_H
#define STD_BOOL_H
#include "../std/bool.h"
#endif

#ifndef COMPILER_TOKEN_H
#define COMPILER_TOKEN_H
#include "token.h"
#endif

#define MAIN_BUFFER_SIZE 243

// Integers enum
typedef enum {
    NAN,
    INT27,
    INT10,
    INT3,
    INTB3,
} INTTYPE;

// Variables enum
typedef enum {
    VSCONST,
    VSTRYTE,
    VSWORD,
    VSTRIPLE,
} SIZE;

// File
intptr file;

// Main buffer
char mBuffer[MAIN_BUFFER_SIZE * 2];

// Interval pointers
char *start;
char *end;

// Check if a character is end of text
bool is_etx() {
    return *end == '\x03' || *end == '\0';
}

// Check if a character is a lowercase letter
bool is_lowercase() {
    return *end >= 'a' && *end <= 'z';
}

// Check if a character is an upper letter
bool is_uppercase() {
    return *end >= 'A' && *end <= 'Z';
}

// Check if a character is a letter
bool is_letter() {
    return is_lowercase() || is_uppercase();
}

// Check if a character is a number
bool is_number() {
    return *end >= '0' && *end <= '9';
}

// Check if a character is a heptavintimal number
bool is_heptavintimal() {
    return is_number() || *end >= 'A' && *end <= 'Q';
}

// Check if a character is a whitespace or \t
bool is_empty() {
    return *end == ' ' || *end == '\t';
}

// Check if a character is \n
bool is_newline() {
    return *end == '\n';
}

// Check if a character is \r
bool is_useless() {
    return *end == '\r';
}

// Check if a character is a balanced ternary number
bool is_balanced() {
    return *end == '!' || *end == '0' || *end == '1';
}

// Check if a character is a ternary number
bool is_ternary() {
    return *end >= '0' && *end <= '2';
}

// Check if a character is part of operator
bool is_operator() {
    return *end == '+' ||
           *end == '-' ||
           *end == '*' ||
           *end == '/' ||
           *end == '%' ||
           *end == '~' ||
           *end == '&' ||
           *end == '|' ||
           *end == '^' ||
           *end == '\\';
}

// Compare string with interval string
bool strcmp_i(const char *a) {
    char *b = start;
    while(*a || b < end) if(*a++ != *b++) return false;
    return true;
}

// Next char
void next() {
    // Jump to next character
    end++;
    if(is_newline()) last.line++;
    else last.column++;
    // If it has reached the end of buffer, load next chars
    if(end >= mBuffer + 2 * MAIN_BUFFER_SIZE) {
        for (uint8_t i = 0; i < MAIN_BUFFER_SIZE; i++)
            mBuffer[i] = mBuffer[i + MAIN_BUFFER_SIZE];
        read(file, mBuffer + 2 * MAIN_BUFFER_SIZE, MAIN_BUFFER_SIZE);
        start -= MAIN_BUFFER_SIZE;
        end -= MAIN_BUFFER_SIZE;
    }
}

// Advance blank spaces
void advance_blank() {
    while(is_empty()) next();
}

// Parse a variable size
bool parse_size() {
    start = end;
    first = last;
    while(is_lowercase()) next();

    if(strcmp_i("const")) {
        make_token(VARSIZE, VSCONST);
        return true;
    }
    if(strcmp_i("tryte")) {
        make_token(VARSIZE, VSTRYTE);
        return true;
    }
    if(strcmp_i("word")) {
        make_token(VARSIZE, VSWORD);
        return true;
    }
    if(strcmp_i("triple")) {
        make_token(VARSIZE, VSTRIPLE);
        return true;
    }
    
    return false;
}

// // Parse a number from string
// INTTYPE parse_number() {
//     // Reset secondary iterator
//     sI = sBuffer;
//     // Detect characters
//     while(is_number(*mI)) {
//         // If there's still space, add char to the secondary buffer
//         if(sI <= sBuffer + SECONDARY_BUFFER_SIZE) sBuffer[*sI++] = *mI;
//         // Next char
//         mI++;
//         // If it has reached the end of the buffer, load next chars
//         if(mI > mBuffer + SECONDARY_BUFFER_SIZE) {
//             read(file, mBuffer, MAIN_BUFFER_SIZE);
//             mI = mBuffer;
//         }
//     }
// }

// // Parse a heptavintimal number from string
// void parse_heptavintimal_number() {
//     // Reset secondary iterator
//     sI = sBuffer;
//     // Detect characters
//     while(is_heptavintimal(*mI)) {
//         // If there's still space, add char to the secondary buffer
//         if(sI <= sBuffer + SECONDARY_BUFFER_SIZE) sBuffer[*sI++] = *mI;
//         // Next char
//         mI++;
//         // If it has reached the end of the buffer, load next chars
//         if(mI > mBuffer + SECONDARY_BUFFER_SIZE) {
//             read(file, mBuffer, MAIN_BUFFER_SIZE);
//             mI = mBuffer;
//         }
//     }
// }

// // Parse a ternary number from string
// void parse_ternary_number() {
//     // Reset secondary iterator
//     sI = sBuffer;
//     // Detect characters
//     while(is_ternary(*mI)) {
//         // If there's still space, add char to the secondary buffer
//         if(sI <= sBuffer + SECONDARY_BUFFER_SIZE) sBuffer[*sI++] = *mI;
//         // Next char
//         mI++;
//         // If it has reached the end of the buffer, load next chars
//         if(mI > mBuffer + SECONDARY_BUFFER_SIZE) {
//             read(file, mBuffer, MAIN_BUFFER_SIZE);
//             mI = mBuffer;
//         }
//     }
// }

// // Parse a balanced ternary number from string
// void parse_balanced_ternary_number() {
//     // Reset secondary iterator
//     sI = sBuffer;
//     // Detect characters
//     while(is_balanced(*mI)) {
//         // If there's still space, add char to the secondary buffer
//         if(sI <= sBuffer + SECONDARY_BUFFER_SIZE) sBuffer[*sI++] = *mI;
//         // Next char
//         mI++;
//         // If it has reached the end of the buffer, load next chars
//         if(mI > mBuffer + SECONDARY_BUFFER_SIZE) {
//             read(file, mBuffer, MAIN_BUFFER_SIZE);
//             mI = mBuffer;
//         }
//     }
// }

// // Parse whitespaces from string
// void skip_whitespaces() {
//     // Skip whitespaces or tabs
//     while(is_empty(*mI)) next();
// }

// // Parse a balanced ternary number from string
// void parse_balanced_ternary_number() {
//     // Reset secondary iterator
//     sI = sBuffer;
//     // Detect characters
//     while(is_balanced(*mI)) {
//         // If there's still space, add char to the secondary buffer
//         if(sI <= sBuffer + SECONDARY_BUFFER_SIZE) sBuffer[*sI++] = *mI;
//         // Next char
//         mI++;
//         // If it has reached the end of the buffer, load next chars
//         if(mI > mBuffer + SECONDARY_BUFFER_SIZE) {
//             read(file, mBuffer, MAIN_BUFFER_SIZE);
//             mI = mBuffer;
//         }
//     }
// }

// // Parse an arithmetic operator
// void parse_arithmetic_operator() {
//     // Reset secondary iterator
//     sI = sBuffer;
//     // Detect characters
//     while(is_balanced(*mI)) {
//         // If there's still space, add char to the secondary buffer
//         if(sI <= sBuffer + SECONDARY_BUFFER_SIZE) sBuffer[*sI++] = *mI;
//         // Next char
//         mI++;
//         // If it has reached the end of the buffer, load next chars
//         if(mI > mBuffer + SECONDARY_BUFFER_SIZE) {
//             read(file, mBuffer, MAIN_BUFFER_SIZE);
//             mI = mBuffer;
//         }
//     }
// }
