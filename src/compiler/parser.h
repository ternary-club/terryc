#ifndef STD_BOOL_H
#define STD_BOOL_H
#include "../std/bool.h"
#endif

#ifndef COMPILER_TOKEN_H
#define COMPILER_TOKEN_H
#include "token.h"
#endif

#ifndef COMPILER_ERROR_H
#define COMPILER_ERROR_H
#include "error.h"
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

// New line
void next_line() {
    last.line++;
    last.column = 0;
}

// New column
void next_column() {
    last.column++;
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
    return is_operator() || is_newline() || is_empty() || is_useless() || is_etx();
}

// Compare string with interval string
bool strcmp_i(const char *a, bool c) {
    char *b = start;
    while(*a || b < end) {
        if(c && 0) puts(ctoa(*a));
        if(*a++ != *b++) return false;
    }
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
    // If its a newline character, then jump to the next line
    if(is_newline()) next_line();
    else next_column();
    // If it has reached the end of buffer, load next chars
    if(end >= mBuffer + 2 * MAIN_BUFFER_SIZE) {
        for (uint8_t i = 0; i < MAIN_BUFFER_SIZE; i++)
            mBuffer[i] = mBuffer[i + MAIN_BUFFER_SIZE];
        read(file, mBuffer + 2 * MAIN_BUFFER_SIZE, MAIN_BUFFER_SIZE);
        start -= MAIN_BUFFER_SIZE;
        end -= MAIN_BUFFER_SIZE;
    }
}

// Previous char
void prev() {
    // Jump to previous character
    end = end == mBuffer ? mBuffer : end - 1;
    // If its a newline character, then jump to the previous line
    if(is_newline()) {
        last.line--;
        last.column = 0;
    }
    else last.column--;
}

// Advance blank spaces
void advance_blank() {
    while(is_empty()) next();
}

// If its a newline character, then jump to the next line
bool parse_newline() {
    if(is_newline()) {
        push(new_token(T_NEWLINE));
        next_line();
        next();
        return true;
    }
    return false;
}

// Parse a variable size
bool parse_size() {   
    begin();    
    
    // Read lowercase letters
    while(is_lowercase()) next();

    // Until it finds a separator
    if(!is_separator()) {
        rewind();
        return false;
    }

    // Then compare the size and push it
    TOKEN t = new_token(T_VARSIZE);
    
    if(strcmp_i("const", true))       *((uint8_t*)t.content) = VS_CONST;
    else if(strcmp_i("tryte", true))  *((uint8_t*)t.content) = VS_TRYTE;
    else if(strcmp_i("word", true))   *((uint8_t*)t.content) = VS_WORD;
    else if(strcmp_i("triple", true)) *((uint8_t*)t.content) = VS_TRIPLE;
    else {
        report_error(E_UNKNOWN_VARSIZE);
        return true;
    }

    push(t);
    return true;
}

// Parse a variable name
bool parse_var() {
    begin();
    // If it doesn't start with an acceptable variable name character,
    // it's likely not a variable
    if(!is_lowercase() && !is_underscore() && !is_number()) return false;

    // Read variable name acceptable characters
    do next(); while(is_lowercase() || is_underscore() || is_number());
    // If it finds a strange character along the way, it's likely a
    // wrong variable name, so read the rest and don't push it
    if(!is_separator()) {
        do next(); while(!is_separator());
        report_error(E_INVALID_VARIABLE_NAME);
        return true;
    }

    // If it doesn't find strange characters, push it
    TOKEN t = new_token(T_NAME);
    strcpy_i(t.content);
    push(t);
    return true;
}

TOKEN parse_token() {
    TOKEN t = new_token(T_NOTOKEN);
    begin();

    // Try to parse base literals
    if(*end == '0') {
        next();
        switch(*end) {
            case 't':
                next();
                while(is_ternary()) next();
                // If it finds a strange character, it's likely
                // a wrong ternary number, so finish it and throw error
                if(!is_separator()) {
                    do next(); while(!is_separator());
                    report_error(E_INVALID_BASE_LITERAL);
                }
                t.tag = T_INT3;
                return t;
            case 'b':
                next();
                while(is_balanced()) next();
                // If it finds a strange character, it's likely
                // a wrong trinary number, so finish it and throw error
                if(!is_separator()) {
                    do next(); while(!is_separator());
                    report_error(E_INVALID_BASE_LITERAL);
                }
                t.tag = T_INTB3;
                return t;
            case 'h':
                next();
                while(is_heptavintimal()) next();
                // If it finds a strange character, it's likely
                // a wrong heptavintimal number, so finish it and throw error
                if(!is_separator()) {
                    do next(); while(!is_separator());
                    report_error(E_INVALID_BASE_LITERAL);
                }
                t.tag = T_INT27;
                return t;
            default:
                // If it doesn't fit anywhere else and if it's a character,
                // then it's an unknown base literal, so finish the string and throw error
                if(is_lowercase()) {
                    do next(); while(!is_separator());
                    report_error(E_UNKNOWN_BASE_LITERAL);
                    t.tag = T_INT10;
                    return t;
                }
                // If it isn't a letter, then it's not a base literal, so
                // get back
                prev();
                break;
        }
    }
    
    // Try to match base 10 number
    if(is_number()) {
        do next(); while(is_number());
        // If it finds a strange character, it's likely
        // a wrong decimal number, so finish it and throw error
        if(!is_separator()) {
            do next(); while(!is_separator());
            report_error(E_INVALID_NUMBER);
        }
        t.tag = T_INT10;
        return t;
    }
    

    // Try to match an operator
    bool hasAssertion = *end == '=';
    if(is_operator() || hasAssertion) {
        bool isOperator = true;
        bool isDiadicTritwise = false;
        // Try to match an assertion
        if(hasAssertion) {
            next();
            if(!is_operator()) {
                t.tag = T_ASSERTION;
                return t;
            }
        }

        // Read operator characters
        do next(); while(!is_operator());

        // Multidic operator
        if(strcmp_i("-", false)) {
            *((uint8_t*)t.content) = M_SUBTRACTION;
            t.tag = T_MULTIDIC;
        }
        // Monadic operators
        else if(strcmp_i("~", false)) {
            *((uint8_t*)t.content) = M_NEGATION;
            t.tag = T_MONADIC;
        } else if(strcmp_i("+/", false)) {
            *((uint8_t*)t.content) = M_INCREMENT;
            t.tag = T_MONADIC;
        } else if(strcmp_i("-/", false)) {
            *((uint8_t*)t.content) = M_DECREMENT;
            t.tag = T_MONADIC;
        } else if(strcmp_i("%|", false)) {
            *((uint8_t*)t.content) = M_ISTRUE;
            t.tag = T_MONADIC;
        } else if(strcmp_i("%/", false)) {
            *((uint8_t*)t.content) = M_ISUNKNOWN;
            t.tag = T_MONADIC;
        } else if(strcmp_i("%-", false)) {
            *((uint8_t*)t.content) = M_ISFALSE;
            t.tag = T_MONADIC;
        } else if(strcmp_i("/\\", false)) {
            *((uint8_t*)t.content) = M_CLAMPUP;
            t.tag = T_MONADIC;
        } else if(strcmp_i("\\/", false)) {
            *((uint8_t*)t.content) = M_CLAMPDOWN;
            t.tag = T_MONADIC;
        }
        // Diadic operators
        else if(strcmp_i("+", false)) {
            *((uint8_t*)t.content) = D_ADDITION;
            t.tag = T_DIADIC;
        } else if(strcmp_i("-", false)) {
            *((uint8_t*)t.content) = D_SUBTRACTION;
            t.tag = T_DIADIC;
        } else if(strcmp_i("*", false)) {
            *((uint8_t*)t.content) = D_MULTIPLICATION;
            t.tag = T_DIADIC;
        } else if(strcmp_i("/", false)) {
            *((uint8_t*)t.content) = D_DIVISION;
            t.tag = T_DIADIC;
        } else if(strcmp_i("%", false)) {
            *((uint8_t*)t.content) = D_MODULO;
            t.tag = T_DIADIC;
        } else if(strcmp_i("|", false)) {
            *((uint8_t*)t.content) = D_OR;
            t.tag = T_DIADIC;
            isDiadicTritwise = true;
        } else if(strcmp_i("&", false)) {
            *((uint8_t*)t.content) = D_AND;
            t.tag = T_DIADIC;
            isDiadicTritwise = true;
        } else if(strcmp_i("^", false)) {
            *((uint8_t*)t.content) = D_XOR;
            t.tag = T_DIADIC;
            isDiadicTritwise = true;
        } else if(strcmp_i("**", false)) {
            *((uint8_t*)t.content) = D_ANY;
            t.tag = T_DIADIC;
            isDiadicTritwise = true;
        } else if(strcmp_i("/%", false)) {
            *((uint8_t*)t.content) = D_CONSENSUS;
            t.tag = T_DIADIC;
            isDiadicTritwise = true;
        } else if(strcmp_i("/+", false)) {
            *((uint8_t*)t.content) = D_SUM;
            t.tag = T_DIADIC;
            isDiadicTritwise = true;
        } else if(strcmp_i("~|", false)) {
            *((uint8_t*)t.content) = D_NOR;
            t.tag = T_DIADIC;
            isDiadicTritwise = true;
        } else if(strcmp_i("~&", false)) {
            *((uint8_t*)t.content) = D_NAND;
            t.tag = T_DIADIC;
            isDiadicTritwise = true;
        } else {
            rewind();
            isOperator = false;
        }
        
        // If it is an operator
        if(isOperator) {
            // and it's logical, then replace the tag
            if(hasAssertion) {
                // If it's not a diadic tritwise operator, throw error
                if(!isDiadicTritwise) report_error(E_LOGICAL_NON_DIADIC_TRITWISE);
                t.tag = T_LOGICAL;
            }
            return t;
        }
    }

    // Try to parse label
    if(is_uppercase()) {
        do next(); while(is_lowercase());
        // If it finds a strange character, it's likely
        // a wrong label name, so finish it and throw error
        if(!is_separator()) {
            do next(); while(!is_separator());
            report_error(E_INVALID_LABEL);
        }
        t.tag = T_LABEL;
        return t;
    }
}