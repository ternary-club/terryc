#ifndef DRIVER_IO_H
#define DRIVER_IO_H
#include "../driver/io.h"
#endif

// Errors enum
typedef enum {
    E_VARDEC_INSIDE_LABEL,
    E_INVALID_VARIABLE_NAME,
    E_UNKNOWN_VARSIZE,
    E_UNEXPECTED_VARSIZE,
    E_EXPECTED_NAME,
    E_EXPECTED_VALUE,
    E_UNKNOWN_BASE_LITERAL,
    E_INVALID_BASE_LITERAL,
    E_INVALID_NUMBER,
    E_LOGICAL_NON_DIADIC_TRITWISE,
    E_INVALID_LABEL,
    E_EXPECTED_MONADIC_OPERAND,
    E_EXPECTED_DIADIC_OPERAND,
} ERROR;

// Report an error
void report_error(ERROR err) {
    puts("\x1b[1m");
    puts(itoa(first.line + 1));
    puts(":");
    puts(itoa(first.column + 1));
    puts(": ");
    puts("\x1b[31merror:\x1b[0m ");
    switch (err) {
    case E_VARDEC_INSIDE_LABEL:
        puts("variable declaration inside label");
        break;
    case E_INVALID_VARIABLE_NAME:
        puts("invalid variable name");
        break;
    case E_UNKNOWN_VARSIZE:
        puts("unknown variable size or constant token");
        break;
    case E_UNEXPECTED_VARSIZE:
        puts("unexpected variable size or constant token");
        break;
    case E_EXPECTED_NAME:
        puts("expected variable name on declaration");
        break;
    case E_EXPECTED_VALUE:
        puts("expected default value after variable declaration");
        break;
    case E_UNKNOWN_BASE_LITERAL:
        puts("unknown base literal");
        break;
    case E_INVALID_BASE_LITERAL:
        puts("invalid algarism for base literal");
        break;
    case E_INVALID_NUMBER:
        puts("invalid algarism in number");
        break;
    case E_LOGICAL_NON_DIADIC_TRITWISE:
        puts("attempt to use logic modifier on a non-diadic tritwise operator");
        break;
    case E_INVALID_LABEL:
        puts("invalid label name");
        break;
    case E_EXPECTED_MONADIC_OPERAND:
        puts("expected operand of monadic operator");
        break;
    case E_EXPECTED_DIADIC_OPERAND:
        puts("expected operand");
        break;
    }
    puts("\n");
}