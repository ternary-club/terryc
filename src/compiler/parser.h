#ifndef STD_BOOL_H
#define STD_BOOL_H
#include "../std/bool.h"
#endif

#ifndef COMPILER_TOKEN_H
#define COMPILER_TOKEN_H
#include "token.h"
#endif

#define MAIN_BUFFER_SIZE 243
#define TOKEN_BUFFER_SIZE 27

// Integers enum
typedef enum {
    I_NAN,
    I_ILITERAL,
    I_INUM,
    I_INT27,
    I_INT10,
    I_INT3,
    I_INTB3,
} INTTYPE;

// Raw values enum
typedef enum {
    R_NOTRAW,
    R_IRAW,
    R_RAW
} RAW;

// File
intptr file;

// Main buffer
char mBuffer[MAIN_BUFFER_SIZE * 2];

// Interval pointers
char *start;
char *end;

// Reset cursor and buffer pointers
void begin() {
    start = end;
    first = last;
}

// Rewind cursor and buffer pointers
void rewind() {
    end = start;
    last = first;
}

// Check if a character is end of text
bool is_etx() {
    return *end == '\x03' || *end == '\0';
}

// Check if a character is an underscore
bool is_underscore() {
    return *end == '_';
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

// Check if a character is a separator
bool is_separator() {
    return is_operator() || is_empty() || is_useless() || is_etx();
}

// Compare string with interval string
bool strcmp_i(const char *a) {
    char *b = start;
    while(*a || b < end) if(*a++ != *b++) return false;
    return true;
}

// Paste interval string in buffer
void strcpy_i(char *a) {
    char *b = start;
    while(b < end && b < start + TOKEN_BUFFER_SIZE) *a++ = *b++;
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
    begin();    
    while(is_lowercase()) next();

    if(!is_separator()) {
        rewind();
        return false;
    }

    TOKEN t = new_token(T_VARSIZE);
    
    if(strcmp_i("const"))       *((uint8_t*)t.content) = VS_CONST;
    else if(strcmp_i("tryte"))  *((uint8_t*)t.content) = VS_TRYTE;
    else if(strcmp_i("word"))   *((uint8_t*)t.content) = VS_WORD;
    else if(strcmp_i("triple")) *((uint8_t*)t.content) = VS_TRIPLE;
    else {
        rewind();
        return false;
    }

    push(t);
    return true;
}

// Parse a monadic operator
bool parse_monadic() {
    begin();
    while(is_operator()) next();

    TOKEN t = new_token(T_MONADIC);
    
    if(strcmp_i("~"))           *((uint8_t*)t.content) = M_NEGATION;
    else if(strcmp_i("+/"))     *((uint8_t*)t.content) = M_INCREMENT;
    else if(strcmp_i("-/"))     *((uint8_t*)t.content) = M_DECREMENT;
    else if(strcmp_i("%|"))     *((uint8_t*)t.content) = M_ISTRUE;
    else if(strcmp_i("%/"))     *((uint8_t*)t.content) = M_ISUNKNOWN;
    else if(strcmp_i("%-"))     *((uint8_t*)t.content) = M_ISFALSE;
    else if(strcmp_i("/\\"))    *((uint8_t*)t.content) = M_CLAMPUP;
    else if(strcmp_i("\\/"))    *((uint8_t*)t.content) = M_CLAMPDOWN;
    else {
        rewind();
        return false;
    }
}

// Parse a variable name
bool parse_var() {
    begin();    
    while(is_lowercase() || is_underscore()) next();

    if(!is_separator()) {
        rewind();
        return false;
    }

    TOKEN t = new_token(T_NAME);
    strcpy_i(t.content);
    push(t);
    return true;
}

// Parse a number from string
INTTYPE parse_number() {
    begin();
    if(*end == '0') {
        switch(*end) {
            case 't':
                while(is_ternary()) next();
                if(!is_separator()) {
                    rewind();
                    return I_INUM;
                }
                return I_INT3;
            case 'b':
                while(is_balanced()) next();
                if(!is_separator()) {
                    rewind();
                    return I_INUM;
                }
                return I_INTB3;
            case 'h':
                while(is_heptavintimal()) next();
                if(!is_separator()) {
                    rewind();
                    return I_INUM;
                }
                return I_INT27;
            default:
                rewind();            
                return I_ILITERAL;
        }
    } else if(is_number()) {
        while(is_number()) next();
        if(!is_separator()) {
            rewind();
            return I_INUM;
        }
        return I_INT10;
    } else return I_NAN;
}

// Parse raw value
RAW parse_raw_value() {
    begin();
    bool hasMonadic = false;
    while(parse_monadic()) hasMonadic = true;

    if(parse_number() || parse_var()) return R_RAW;
    rewind();
    return hasMonadic ? R_IRAW : R_NOTRAW;
}

// Parse a value
bool parse_value() {
    begin();
    if(parse_number() || parse_var()) {

    }
}