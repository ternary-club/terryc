#ifndef STD_INT_H
#define STD_INT_H
#include "../std/int.h"
#endif


#ifndef DRIVER_MEM_H
#define DRIVER_MEM_H
#include "../driver/mem.h"
#endif

// Tags enum
typedef enum {
    T_NOTOKEN,
    T_NEWLINE,
    T_MONADIC,
    T_DIADIC,
    T_LABEL,
    T_NUMBER,
    T_VARSIZE,
    T_NAME,
    T_REGISTER,
    T_ASSERTION,
    T_COMPARISON,
    T_ENDPOINT
} TAG;

// Variables enum
typedef enum {
    VS_CONST,
    VS_TRYTE,
    VS_WORD,
    VS_TRIPLE,
} SIZE;

// Monadic operators enum
typedef enum {
    M_NEGATION,
    M_INCREMENT,
    M_DECREMENT,
    M_ISTRUE,
    M_ISUNKNOWN,
    M_ISFALSE,
    M_CLAMPUP,
    M_CLAMPDOWN
} MONADIC;

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
    char content[27];
} TOKEN;

// Stack
TOKEN *stack = 0;
uint64_t height = 0;

// Cursor
COORDINATE first;
COORDINATE last;

// Push token into stack
void push(TOKEN t) {
    if(!height++) stack = (TOKEN*)heap();
    *((TOKEN*)alloc(sizeof(TOKEN))) = t;
}

// Pop token from stack
TOKEN pop() {
    TOKEN t = *(TOKEN*)((uint64_t)heap() - sizeof(TOKEN));
    free(sizeof(TOKEN));
    height--;
    return t;
}

// View top item of the stack
TOKEN peep() {
   return height ? *(TOKEN*)((uint64_t)heap() - sizeof(TOKEN)) : (TOKEN){T_NOTOKEN};
}

// New token from tag
TOKEN new_token(TAG t) {
    return (TOKEN){
        t,
        first,
        last,
    };
}