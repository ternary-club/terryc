#ifndef DRIVER_IO_H
#define DRIVER_IO_H
#include "../driver/io.h"
#endif

#ifndef STD_STR_H
#define STD_STR_H
#include "../std/str.h"
#endif

// Errors enum
typedef enum {
    E_VARDEC_INSIDE_LABEL,
    E_COMMAND_OUTSIDE_LABEL,
    E_ASSERTION_OUTSIDE_LABEL,
    E_INVALID_NAME,
    E_EXPECTED_NAME_VARDEC,
    E_UNKNOWN_BASE_LITERAL,
    E_INVALID_BASE_LITERAL,
    E_EMPTY_BASE_LITERAL,
    E_READ_NUMBER,
    E_UNEXPECTED_NUMBER,
    E_INVALID_NUMBER,
    E_UNEXPECTED_QUATERNARY_OPERATOR,
    E_INVALID_QUATERNARY_RETURN_TYPES,
    E_MULTIPLE_QUATERNARY_RETURN_TYPES,
    E_UNEXPECTED_ELSE_OUTCOME,
    E_MULTIPLE_OUTCOME,
    E_EXPECTED_OUTCOME,
    E_UNEXPECTED_OUTCOME,
    E_INVALID_OUTCOME,
    E_EXPECTED_LABEL,
    E_READ_LABEL,
    E_INVALID_LABEL,
    E_INVALID_RESERVED_LABEL,
    E_EXPECTED_END,
    E_READ_REGISTER,
    E_EXPECTED_REGISTER,
    E_UNKNOWN_REGISTER,
    E_EXPECTED_ASSERTION,
    E_UNTARGETED_ASSERTION,
    E_READ_OPERATOR,
    E_UNEXPECTED_OPERATOR,
    E_UNSPACED_OPERATOR,
    E_UNKNOWN_OPERATOR,
    E_LOGICAL_NON_DIADIC_TRITWISE,
    E_LOGICAL_MONADIC_ASSERTION_OPERATOR,
    E_EXPECTED_OPERAND,
    E_EXPECTED_VALUE,
    E_UNKNOWN_TOKEN,
} ERROR;

// Source filename
const char *filename = NULL;

// Report a warning (compiler-wise)
void report_warning(const char *warning, bool newLine) {
    puts("\x1b[33;1mwarning:\x1b[0m ");
    puts(warning);
    if (newLine) puts("\n");
}

// Report an error
void report_error(ERROR err) {
    puts("\x1b[1m");
    puts(filename);
    puts(":");
    puts(itoa(first.line + 1));
    puts(":");
    puts(itoa(first.column + 1));
    puts(": ");
    puts("\x1b[31merror:\x1b[0m ");
    switch (err) {
        case E_VARDEC_INSIDE_LABEL:
            puts("variable declaration inside label");
            break;
        case E_COMMAND_OUTSIDE_LABEL:
            puts("command outside label");
            break;
        case E_ASSERTION_OUTSIDE_LABEL:
            puts("assertion outside label");
            break;
        case E_INVALID_NAME:
            puts("invalid variable name");
            break;
        case E_EXPECTED_NAME_VARDEC:
            puts("expected variable name on declaration");
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
        case E_READ_NUMBER:
            puts("cannot use number as value");
            break;
        case E_UNEXPECTED_NUMBER:
            puts("unexpected number");
            break;
        case E_INVALID_NUMBER:
            puts("invalid algarism in number");
            break;
        case E_UNEXPECTED_QUATERNARY_OPERATOR:
            puts(
                "cannot use a quaternary operator after a non-logical "
                "operation");
            break;
        case E_INVALID_QUATERNARY_RETURN_TYPES:
            puts(
                "return value type of quaternary operator differs from outer "
                "equation");
            break;
        case E_MULTIPLE_QUATERNARY_RETURN_TYPES:
            puts(
                "cannot return different types of values from a quaternary "
                "operation");
            break;
        case E_UNEXPECTED_ELSE_OUTCOME:
            puts("':' cannot be the first outcome");
            break;
        case E_MULTIPLE_OUTCOME:
            puts("cannot more than one outcome that leads to the same result");
            break;
        case E_EXPECTED_OUTCOME:
            puts("expected quaternary operator outcome");
            break;
        case E_UNEXPECTED_OUTCOME:
            puts("unexpected quaternary operator outcome");
            break;
        case E_INVALID_OUTCOME:
            puts("invalid outcome");
            break;
        case E_READ_LABEL:
            puts("cannot use label as value");
            break;
        case E_EXPECTED_LABEL:
            puts("expected label");
            break;
        case E_INVALID_LABEL:
            puts("invalid label name");
            break;
        case E_INVALID_RESERVED_LABEL:
            puts("cannot declare a reserved label");
            break;
        case E_EXPECTED_END:
            puts("expected end of statement");
            break;
        case E_READ_REGISTER:
            puts("cannot use register as value in a constant equation");
            break;
        case E_EXPECTED_REGISTER:
            puts("expected register");
            break;
        case E_UNKNOWN_REGISTER:
            puts("unknown register");
            break;
        case E_EXPECTED_ASSERTION:
            puts("expected assertion");
            break;
        case E_UNTARGETED_ASSERTION:
            puts("no target found before assertion");
            break;
        case E_EXPECTED_OPERAND:
            puts("expected operand");
            break;
        case E_EXPECTED_VALUE:
            puts("expected value");
            break;
        case E_READ_OPERATOR:
            puts("cannot use operator");
            break;
        case E_UNEXPECTED_OPERATOR:
            puts("unexpected operator");
            break;
        case E_UNSPACED_OPERATOR:
            puts("expected separator before operand");
            break;
        case E_UNKNOWN_OPERATOR:
            puts("unknown operator");
            break;
        case E_LOGICAL_NON_DIADIC_TRITWISE:
            puts("cannot use logic modifier on non-diadic tritwise operator");
            break;
        case E_LOGICAL_MONADIC_ASSERTION_OPERATOR:
            puts("assertion operator cannot be monadic or logical");
            break;
        case E_UNKNOWN_TOKEN:
            puts("unknown token");
            break;
    }
    puts("\n");
}
