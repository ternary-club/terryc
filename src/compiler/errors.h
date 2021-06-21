#ifndef DRIVER_IO_H
#define DRIVER_IO_H
#include "../driver/io.h"
#endif

// Errors enum
typedef enum {
    E_VARDEC_INSIDE_LABEL,
    E_UNEXPECTED_VARSIZE,
    E_EXPECTED_NAME,
    E_EXPECTED_VALUE,
    E_INVALID_BASE_LITERAL
} ERROR;

// Report an error
void report_error(ERROR err) {
    puts("error: ");
    switch (err) {
    case E_VARDEC_INSIDE_LABEL:
        puts("variable declaration inside label");
        break;
    case E_UNEXPECTED_VARSIZE:
        puts("unexpected variable size or constant token");
        break;
    case E_EXPECTED_NAME:
        puts("expected variable name on declaration");
        break;
    case E_EXPECTED_VALUE:
        puts("expected default variable value on declaration");
        break;
    case E_INVALID_BASE_LITERAL:
        puts("invalid base literal");
        break;
    }
    puts(" at ");
    puts(itoa(first.line));
    puts(",");
    puts(itoa(first.column));
    puts("\n");
}