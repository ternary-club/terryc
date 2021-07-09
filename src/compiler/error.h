#ifndef DRIVER_IO_H
#define DRIVER_IO_H
#include "../driver/io.h"
#endif

// Errors enum
typedef enum {
    E_VARDEC_INSIDE_LABEL,
    E_INVALID_NAME,
    E_UNEXPECTED_NAME,
    E_UNKNOWN_VARSIZE,
    E_UNEXPECTED_VARSIZE,
    E_EXPECTED_NAME_VARDEC,
    E_EXPECTED_VALUE_VARDEC,
    E_UNKNOWN_BASE_LITERAL,
    E_INVALID_BASE_LITERAL,
    E_EMPTY_BASE_LITERAL,
    E_UNEXPECTED_VALUE,
    E_INVALID_NUMBER,
    E_LOGICAL_NON_DIADIC_TRITWISE,
    E_READ_LABEL,
    E_INVALID_LABEL,
    E_UNEXPECTED_LABEL,
    E_UNEXPECTED_LABEL_TOKEN,
    E_READ_REGISTER,
    E_UNEXPECTED_REGISTER,
    E_UNKNOWN_REGISTER,
    E_EXPECTED_ASSERTION,
    E_UNEXPECTED_ASSERTION,
    E_INVALID_ASSERTION,
    E_EXPECTED_OPERATOR,
    E_UNEXPECTED_OPERATOR,
    E_EXPECTED_OPERAND,
    E_UNKNOWN_TOKEN,
} ERROR;

// Report an error
void report_error(ERROR err) {
    puts("\x1b[1m");
    puts(itoa(first.line + 1));
    puts(":");
    puts(itoa(first.column + 1));
    puts(":\t");
    puts("\x1b[31merror:\x1b[0m ");
    switch (err) {
    case E_VARDEC_INSIDE_LABEL:
        puts("variable declaration inside label");
        break;
    case E_INVALID_NAME:
        puts("invalid variable name");
        break;
    case E_UNEXPECTED_NAME:
        puts("unexpected variable");
        break;
    case E_UNKNOWN_VARSIZE:
        puts("unknown variable size or constant token");
        break;
    case E_UNEXPECTED_VARSIZE:
        puts("unexpected variable size or constant token");
        break;
    case E_EXPECTED_NAME_VARDEC:
        puts("expected variable name on declaration");
        break;
    case E_EXPECTED_VALUE_VARDEC:
        puts("expected default value after variable declaration");
        break;
    case E_UNKNOWN_BASE_LITERAL:
        puts("unknown base literal");
        break;
    case E_INVALID_BASE_LITERAL:
        puts("invalid algarism for base literal");
        break;
    case E_EMPTY_BASE_LITERAL:
        puts("empty number in base literal");
        break;
    case E_UNEXPECTED_VALUE:
        puts("unexpected value");
        break;
    case E_INVALID_NUMBER:
        puts("invalid algarism in number");
        break;
    case E_LOGICAL_NON_DIADIC_TRITWISE:
        puts("cannot use logic modifier on non-diadic tritwise operator");
        break;
    case E_READ_LABEL:
        puts("cannot use label as value");
        break;
    case E_INVALID_LABEL:
        puts("invalid label name");
        break;
    case E_UNEXPECTED_LABEL:
        puts("unexpected label");
        break;
    case E_UNEXPECTED_LABEL_TOKEN:
        puts("unexpected token after label");
        break;
    case E_READ_REGISTER:
        puts("cannot use register as value");
        break;
    case E_UNEXPECTED_REGISTER:
        puts("unexpected register");
        break;
    case E_UNKNOWN_REGISTER:
        puts("unknown register");
        break;
    case E_EXPECTED_ASSERTION:
        puts("expected value assertion to variable or register");
        break;
    case E_UNEXPECTED_ASSERTION:
        puts("unexpected assertion");
        break;
    case E_INVALID_ASSERTION:
        // puts("invalid assertion to non-register or variable token");
        break;
    case E_EXPECTED_OPERAND:
        puts("expected operand");
        break;
    case E_UNEXPECTED_OPERATOR:
        puts("unexpected operator");
        break;
    case E_EXPECTED_OPERATOR:
        puts("expected operator");
        break;
    case E_UNKNOWN_TOKEN:
        puts("unknown token");
        break;
    }
    puts("\n");
}