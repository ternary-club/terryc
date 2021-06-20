#ifndef STD_INT_H
#define STD_INT_H
#include "../std/int.h"
#endif


#ifndef DRIVER_MEM_H
#define DRIVER_MEM_H
#include "../driver/mem.h"
#endif

typedef enum {
    NOTOKEN,
    LABEL,
    NUMBER,
    VARSIZE,
    NAME,
    REGISTER,
    ASSERTION,
    COMPARISON,
    ENDPOINT
} TAG;

typedef struct {
    uint8_t line;
    uint8_t column;
} COORDINATE;

typedef struct {
    TAG tag;
    uint16_t content;
    COORDINATE start;
    COORDINATE end;
} TOKEN;

// Stack
TOKEN *stack = 0;

// Cursor
COORDINATE first;
COORDINATE last;

// Push token into stack
void push(TOKEN t) {
    if(stack->tag == NOTOKEN) stack = (TOKEN*)heap();
    *((TOKEN*)alloc(sizeof(TOKEN))) = t;
}

// Pop token from stack
TOKEN pop() {
    TOKEN t = *(TOKEN*)((uint64_t)heap() - sizeof(TOKEN));
    free(sizeof(TOKEN));
    return t;
}

// Make token from tag and insert it into stack
void make_token(TAG t, uint64_t content) {
    push((TOKEN){t, content, first, last});
}