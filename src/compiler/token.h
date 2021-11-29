#ifndef STD_INT_H
#define STD_INT_H
#include "../std/int.h"
#endif

#ifndef STD_STR_H
#define STD_STR_H
#include "../std/str.h"
#endif

#ifndef STD_tryte_H
#define STD_tryte_H
#include "../std/tryte.h"
#endif

#ifndef DRIVER_MEM_H
#define DRIVER_MEM_H
#include "../driver/mem.h"
#endif

// Tags enum
typedef enum {
    // Nil tokens
    T_NOTOKEN,
    T_NEWLINE,
    T_ENDPOINT,
    // Arithmetic operators
    T_MONADIC,
    T_DIADIC,
    T_MULTIDIC,
    T_LOGICAL,
    // Logical structures
    T_QUATERNARY,
    T_OUTCOME,
    // User
    T_LABEL,
    T_NAME,
    // Values
    T_INTB3,
    T_INT3,
    T_INT10,
    T_INT27,
    // Keywords
    T_VARSIZE,
    T_REGISTER,
    // Actions
    T_ASSERTION,
    T_COMMAND,
} TAG;

// Labels enum
typedef enum {
    L_CUSTOM,
    L_CONTINUE,
    L_END,
} LABEL;

// Variables enum
typedef enum {
    VS_CONST,
    VS_TRYTE,
    VS_WORD,
    VS_TRIPLE,
} SIZE;

// Outcome structures
typedef enum {
    O_ELSE = 0,
    O_EQUAL = 1,
    O_GREATER = 1 << 1,
    O_LESS = 1 << 2,
    O_IGNORE = 0xff  // For error correction purposes
} OUTCOME;

// Monadic operators enum
typedef enum {
    M_SUBTRACTION,
    M_NEGATION,
    M_INCREMENT,
    M_DECREMENT,
    M_ISTRUE,
    M_ISUNKNOWN,
    M_ISFALSE,
    M_CLAMPUP,
    M_CLAMPDOWN
} MONADIC;

// Diadic operators enum
typedef enum {
    D_ADDITION,
    D_SUBTRACTION,
    D_MULTIPLICATION,
    D_DIVISION,
    D_MODULO,
    D_OR,
    D_AND,
    D_XOR,
    D_ANY,
    D_CONSENSUS,
    D_SUM,
    D_NOR,
    D_NAND,
} DIADIC;

// Commands enum
typedef enum {
    C_CALL,
    C_GOTO,
} COMMAND;

// Coordinate struct
typedef struct {
    uint16_t line;
    uint16_t column;
} COORDINATE;

// Token struct
typedef struct {
    TAG tag;
    COORDINATE start;
    COORDINATE end;
    uint8_t content[32];
} TOKEN;

// Stack
TOKEN *stack = 0;
uint64_t height = 0;

// Cursor
COORDINATE first;
COORDINATE last;

// Empty default token
#define NEW_TOKEN (TOKEN){T_NOTOKEN, first, last};

// Push token into stack
void push(TOKEN t) {
    if (!height++) stack = (TOKEN *)heap();
    *((TOKEN *)alloc(sizeof(TOKEN))) = t;
}

// Pop token from stack
TOKEN pop() {
    TOKEN t = *(TOKEN *)((uint64_t)heap() - sizeof(TOKEN));
    free(sizeof(TOKEN));
    height--;
    return t;
}

// View top item of stack
TOKEN peep() {
    return height ? *(TOKEN *)((uint64_t)heap() - sizeof(TOKEN))
                  : (TOKEN){T_NOTOKEN};
}

void putt(TAG tag) {
    switch (tag) {
        default:
            puts("UNKNOWN");
            break;
        case T_ENDPOINT:
            puts(".");
            break;
        case T_NOTOKEN:
            puts("INVALID");
            break;
        case T_NEWLINE:
            puts("\n");
            break;
        case T_MONADIC:
            puts("monadic");
            break;
        case T_DIADIC:
            puts("diadic");
            break;
        case T_MULTIDIC:
            puts("multidic");
            break;
        case T_QUATERNARY:
            puts("?");
            break;
        case T_OUTCOME:
            puts("outcome");
            break;
        case T_LOGICAL:
            puts("logical");
            break;
        case T_LABEL:
            puts("label");
            break;
        case T_INTB3:
            puts("intb3");
            break;
        case T_INT3:
            puts("int3");
            break;
        case T_INT10:
            puts("int");
            break;
        case T_INT27:
            puts("int27");
            break;
        case T_VARSIZE:
            puts("size");
            break;
        case T_NAME:
            puts("var");
            break;
        case T_REGISTER:
            puts("reg");
            break;
        case T_ASSERTION:
            puts("=");
            break;
        case T_COMMAND:
            puts("comm");
            break;
    }
    // puts("(");
    // puts(itoa(tag));
    // puts(")");
}