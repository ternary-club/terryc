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
void next_column() { last.column++; }

// Check if a character is end of text
bool is_etx() { return *end == '\x03' || *end == '\0'; }

// Check if a character is the beginning of a commentary
bool is_comment() { return *end == '#'; }

// Check if a character is an underscore
bool is_underscore() { return *end == '_'; }

// Check if a character is a lowercase letter
bool is_lowercase() { return *end >= 'a' && *end <= 'z'; }

// Check if a character is an upper letter
bool is_uppercase() { return *end >= 'A' && *end <= 'Z'; }

// Check if a character is a letter
bool is_letter() { return is_lowercase() || is_uppercase(); }

// Check if a character is a number
bool is_number() { return *end >= '0' && *end <= '9'; }

// Check if a character is a heptavintimal number
bool is_heptavintimal() { return is_number() || *end >= 'A' && *end <= 'Q'; }

// Check if a character is a whitespace or \t
bool is_empty() { return *end == ' ' || *end == '\t'; }

// Check if a character is CR
bool is_carriage() { return *end == '\r'; }

// Check if a character is LF
bool is_newline() { return *end == '\n'; }

// Check if a character is CR/LF
bool is_line_end() { return is_newline() || is_carriage(); }

// Check if a character is a balanced ternary number
bool is_balanced() { return *end == 'N' || *end == '0' || *end == '1'; }

// Check if a character is a ternary number
bool is_ternary() { return *end >= '0' && *end <= '2'; }

// Check if a character is zero
bool is_zero() { return *end == '0'; }

// Check if a character is an assertion
bool is_assertion() { return *end == '='; }

// Check if a character is part of operator
bool is_operator() {
    return *end == '+' || *end == '-' || *end == '*' || *end == '/' ||
           *end == '%' || *end == '~' || *end == '&' || *end == '|' ||
           *end == '^' || *end == '\\';
}

// Check if a character is a separator
bool is_separator() { return is_line_end() || is_empty() || is_etx(); }

// Compare string with interval string
bool strcmp_i(const char *a) {
    char *b = start;
    while (*a || b < end)
        if (*a++ != *b++) return false;
    return true;
}

// Paste interval string in buffer
void strcpy_i(char *a) {
    char *b = start;
    while (b < end && b < start + TOKEN_BUFFER_SIZE) *a++ = *b++;
}

// Next char
void next() {
    // If its a newline character, then jump to the next line
    if (is_newline())
        next_line();
    else if (!is_carriage())
        next_column();
    // Jump to next character
    end++;
    // If it has reached the end of buffer, load next chars
    if (end >= mBuffer + 2 * MAIN_BUFFER_SIZE) {
        for (uint8_t i = 0; i < MAIN_BUFFER_SIZE; i++)
            mBuffer[i] = mBuffer[i + MAIN_BUFFER_SIZE];
        read(file, mBuffer + 2 * MAIN_BUFFER_SIZE, MAIN_BUFFER_SIZE);
        start -= MAIN_BUFFER_SIZE;
        end -= MAIN_BUFFER_SIZE;
    }
}

// Detect token
TOKEN parse_token() {
    // Current token
    TOKEN t = NEW_TOKEN;

    // Save cursor's checkpoint
    begin();

    // Try to parse a comment
    if (is_comment()) do
            next();
        while (!is_line_end() && !is_etx());

    // Try to parse a newline
    if (is_line_end()) {
        do next();
        while (is_line_end());
        t.tag = T_NEWLINE;
        return t;
    }

    // Try to parse the end of text
    if (is_etx()) {
        t.tag = T_ENDPOINT;
        return t;
    }

    // Try to parse base literals
    if (is_zero()) {
        next();
        switch (*end) {
            case 't':
                next();
                // If it has got 0 algarism, then throw error
                if (is_separator())
                    report_error(E_EMPTY_BASE_LITERAL);
                else
                    while (is_ternary()) next();
                // If it finds a strange character, it's likely
                // a wrong ternary number, so finish it and throw error
                if (!is_separator()) {
                    do next();
                    while (!is_separator());
                    report_error(E_INVALID_BASE_LITERAL);
                }
                t.tag = T_INT3;
                return t;
            case 'b':
                next();
                // If it has got 0 algarism, then throw error
                if (is_separator())
                    report_error(E_EMPTY_BASE_LITERAL);
                else
                    while (is_balanced()) next();
                // If it finds a strange character, it's likely
                // a wrong trinary number, so finish it and throw error
                if (!is_separator()) {
                    do next();
                    while (!is_separator());
                    report_error(E_INVALID_BASE_LITERAL);
                }
                t.tag = T_INTB3;
                return t;
            case 'h':
                next();
                // If it has got 0 algarism, then throw error
                if (is_separator())
                    report_error(E_EMPTY_BASE_LITERAL);
                else
                    while (is_heptavintimal()) next();
                // If it finds a strange character, it's likely
                // a wrong heptavintimal number, so finish it and throw error
                if (!is_separator()) {
                    do next();
                    while (!is_separator());
                    report_error(E_INVALID_BASE_LITERAL);
                }
                t.tag = T_INT27;
                return t;
            default:
                // If it doesn't fit anywhere else and if it's a character,
                // then it's an unknown base literal, so finish the string and
                // throw error
                if (is_lowercase()) {
                    report_error(E_UNKNOWN_BASE_LITERAL);
                    next();
                    // If it has got 0 algarism, then throw error
                    if (is_separator())
                        report_error(E_EMPTY_BASE_LITERAL);
                    else
                        while (!is_separator()) next();
                    t.tag = T_INT10;
                    return t;
                }
                // If it isn't a letter, then it's not a base literal, so
                // get back
                rewind();
                break;
        }
    }

    // Try to parse register
    if (is_number()) {
        bool isExistingRegister = true;
        int8_t registerNum = *end - '0';
        // If it's out of the registers intervals, then it's a non-existing
        // register
        if (registerNum < 1 || registerNum > 3 && registerNum != 9)
            isExistingRegister = false;

        next();
        if (is_lowercase()) {
            char registerLetter = *end - 'a';
            // If it's out of the registers intervals, then it's a non-existing
            // register
            if (isExistingRegister &&
                    (registerLetter < 0 || registerLetter > 8) ||
                (registerLetter > 2) && registerNum == 9)
                isExistingRegister = false;

            next();
            // If it's exactly a number and a letter, then it's
            // likely a register
            if (is_separator()) {
                if (!isExistingRegister) report_error(E_UNKNOWN_REGISTER);
                t.tag = T_REGISTER;
                return t;
            } else
                rewind();
        } else
            rewind();
    }

    // Try to parse base 10 number
    if (is_number()) {
        do next();
        while (is_number());
        // If it finds a strange character, it's likely
        // a wrong decimal number, so finish it and throw error
        if (!is_separator()) {
            do next();
            while (!is_separator());
            report_error(E_INVALID_NUMBER);
        }
        t.tag = T_INT10;
        return t;
    }

    // Try to parse an assertion
    bool isLogical = false;
    if (is_assertion()) {
        next();
        if (!is_operator()) {
            t.tag = T_ASSERTION;
            // Represents no operator
            *((uint8_t *)t.content) = 0;
            return t;
        } else
            isLogical = true;
    }

    // Try to parse an operator
    if (is_operator()) {
        // If the operator is diadic and tritwise (both necessary for it to be
        // logical)
        bool isDiadicTritwise = false;

        // Read operator characters
        do next();
        while (is_operator());

        // Multidic operator
        if (strcmp_i("-")) {
            *((uint8_t *)t.content) = M_SUBTRACTION;
            t.tag = T_MULTIDIC;
        }

        // Monadic operators
        else if (strcmp_i("~")) {
            *((uint8_t *)t.content) = M_NEGATION;
            t.tag = T_MONADIC;
        } else if (strcmp_i("+/")) {
            *((uint8_t *)t.content) = M_INCREMENT;
            t.tag = T_MONADIC;
        } else if (strcmp_i("-/")) {
            *((uint8_t *)t.content) = M_DECREMENT;
            t.tag = T_MONADIC;
        } else if (strcmp_i("%|")) {
            *((uint8_t *)t.content) = M_ISTRUE;
            t.tag = T_MONADIC;
        } else if (strcmp_i("%/")) {
            *((uint8_t *)t.content) = M_ISUNKNOWN;
            t.tag = T_MONADIC;
        } else if (strcmp_i("%-")) {
            *((uint8_t *)t.content) = M_ISFALSE;
            t.tag = T_MONADIC;
        } else if (strcmp_i("/\\")) {
            *((uint8_t *)t.content) = M_CLAMPUP;
            t.tag = T_MONADIC;
        } else if (strcmp_i("\\/")) {
            *((uint8_t *)t.content) = M_CLAMPDOWN;
            t.tag = T_MONADIC;
        }

        // Diadic operators
        else if (strcmp_i("+")) {
            *((uint8_t *)t.content) = D_ADDITION;
            t.tag = T_DIADIC;
        } else if (strcmp_i("-")) {
            *((uint8_t *)t.content) = D_SUBTRACTION;
            t.tag = T_DIADIC;
        } else if (strcmp_i("*")) {
            *((uint8_t *)t.content) = D_MULTIPLICATION;
            t.tag = T_DIADIC;
        } else if (strcmp_i("/")) {
            *((uint8_t *)t.content) = D_DIVISION;
            t.tag = T_DIADIC;
        } else if (strcmp_i("%")) {
            *((uint8_t *)t.content) = D_MODULO;
            t.tag = T_DIADIC;
        } else if (strcmp_i("|")) {
            *((uint8_t *)t.content) = D_OR;
            t.tag = T_DIADIC;
            isDiadicTritwise = true;
        } else if (strcmp_i("&")) {
            *((uint8_t *)t.content) = D_AND;
            t.tag = T_DIADIC;
            isDiadicTritwise = true;
        } else if (strcmp_i("^")) {
            *((uint8_t *)t.content) = D_XOR;
            t.tag = T_DIADIC;
            isDiadicTritwise = true;
        } else if (strcmp_i("**")) {
            *((uint8_t *)t.content) = D_ANY;
            t.tag = T_DIADIC;
            isDiadicTritwise = true;
        } else if (strcmp_i("/%")) {
            *((uint8_t *)t.content) = D_CONSENSUS;
            t.tag = T_DIADIC;
            isDiadicTritwise = true;
        } else if (strcmp_i("/+")) {
            *((uint8_t *)t.content) = D_SUM;
            t.tag = T_DIADIC;
            isDiadicTritwise = true;
        } else if (strcmp_i("~|")) {
            *((uint8_t *)t.content) = D_NOR;
            t.tag = T_DIADIC;
            isDiadicTritwise = true;
        } else if (strcmp_i("~&")) {
            *((uint8_t *)t.content) = D_NAND;
            t.tag = T_DIADIC;
            isDiadicTritwise = true;
        }

        // Unknown
        else {
            // Throw error
            report_error(E_UNKNOWN_OPERATOR);
            // Invalid operators are treated as multidic so they don't cause
            // additional inter token problems
            t.tag = T_MULTIDIC;
        }

        // If the final token is an assertion, it means it's an assertion
        // operator
        if (is_assertion()) {
            // Throw error
            if (t.tag != T_DIADIC || isLogical)
                report_error(E_LOGICAL_MONADIC_ASSERTION_OPERATOR);
            t.tag = T_ASSERTION;
            // Advance
            next();
        }
        // If it's a logical operator, replace the tag
        else {
            if (isLogical) {
                // If it's not a diadic tritwise operator, throw error
                if (!isDiadicTritwise)
                    report_error(E_LOGICAL_NON_DIADIC_TRITWISE);
                t.tag = T_LOGICAL;
            }
            // Check if operators are separated
            if (!is_separator()) report_error(E_UNSPACED_OPERATOR);
        }

        return t;
    }

    // Try to parse label
    if (is_uppercase()) {
        do next();
        while (is_lowercase() || is_number() || is_uppercase());
        // If it finds a strange character, it's likely
        // a wrong label name, so finish it and throw error
        if (!is_separator()) {
            do next();
            while (!is_separator());
            report_error(E_INVALID_LABEL);
        }

        // Check if label is reserved
        if (strcmp_i("Continue"))
            *((uint8_t *)t.content) = L_CONTINUE;
        else if (strcmp_i("End"))
            *((uint8_t *)t.content) = L_END;
        else
            *((uint8_t *)t.content) = L_CUSTOM;

        t.tag = T_LABEL;
        return t;
    }

    // Try to parse variable size
    if (is_lowercase()) {
        bool isVarSize = true;
        do next();
        while (is_lowercase());

        // If it finds a strange character, it's likely not
        // a wrong variable size, so mark it as false
        if (!is_separator()) isVarSize = false;

        // Try to compare variable sizes
        else if (strcmp_i("const"))
            *((uint8_t *)t.content) = VS_CONST;
        else if (strcmp_i("tryte"))
            *((uint8_t *)t.content) = VS_TRYTE;
        else if (strcmp_i("word"))
            *((uint8_t *)t.content) = VS_WORD;
        else if (strcmp_i("triple"))
            *((uint8_t *)t.content) = VS_TRIPLE;
        else
            isVarSize = false;

        // Rewind if is not a known variable size or return if it is
        if (isVarSize) {
            t.tag = T_VARSIZE;
            return t;
        } else
            rewind();
    }

    // Try to parse command
    if (is_lowercase()) {
        bool isCommand = true;
        do next();
        while (is_lowercase());

        // If it finds a strange character, it's likely not
        // a command, so mark it as false
        if (!is_separator()) isCommand = false;

        // Try to compare commands
        else if (strcmp_i("call"))
            *((uint8_t *)t.content) = C_CALL;
        else if (strcmp_i("goto"))
            *((uint8_t *)t.content) = C_GOTO;
        else
            isCommand = false;

        // Rewind if is not a known variable size or return if it is
        if (isCommand) {
            t.tag = T_COMMAND;
            return t;
        } else
            rewind();
    }

    // Try to parse variable name
    if (is_lowercase() || is_underscore()) {
        do next();
        while (is_lowercase() || is_underscore() || is_number());
        // If it doesn't fit anywhere else and if it's a character, it's likely
        // a bad variable name, so finish the string, throw error and return
        if (!is_separator()) {
            do next();
            while (!is_separator());
            report_error(E_INVALID_NAME);
        }
        t.tag = T_NAME;
        return t;
    }

    // If it's nothing else, then return T_NOTOKEN
    do next();
    while (!is_separator() && !is_assertion() && !is_number() && !is_letter() &&
           !is_comment() && !is_operator());
    t.tag = T_NOTOKEN;
    return t;
}