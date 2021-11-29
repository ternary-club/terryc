#ifndef COMPILER_TOKEN_H
#define COMPILER_TOKEN_H
#include "token.h"
#endif

#ifndef COMPILER_ERROR_H
#define COMPILER_ERROR_H
#include "error.h"
#endif

#ifndef STD_STR_H
#define STD_STR_H
#include "../std/str.h"
#endif

#ifndef STD_TRYTE_H
#define STD_TRYTE_H
#include "../std/tryte.h"
#endif

#ifndef STD_BOOL_H
#define STD_BOOL_H
#include "../std/bool.h"
#endif

#ifndef STD_OP_H
#define STD_OP_H
#include "../std/op.h"
#endif

#ifndef MESA_TRYTE_H
#define MESA_TRYTE_H
#include "../mesa/tryte.h"
#endif

#ifndef MESA_LOGIC_H
#define MESA_LOGIC_H
#include "../mesa/logic.h"
#endif

// Default memory is 3MtT
#define MEMORY_SIZE (3 * METRI)

// Memory
__tryte_buffer(memory, MEMORY_SIZE);

// Named address struct
typedef struct {
    char *name;
    uint64_t addr;
} NAMED_ADDR;

// Label stack
NAMED_ADDR *labStack = 0;
uint64_t labHeight = 0;

// Variable stack
NAMED_ADDR *varStack = 0;
uint64_t varHeight = 0;

// Push label into stack
void push_label(NAMED_ADDR label) {
    if (!labHeight++) labStack = (NAMED_ADDR *)heap();
    *((NAMED_ADDR *)alloc(sizeof(NAMED_ADDR))) = label;
}

// Push variable into stack
void push_variable(NAMED_ADDR var) {
    if (!varHeight++) varStack = (NAMED_ADDR *)heap();
    *((NAMED_ADDR *)alloc(sizeof(NAMED_ADDR))) = var;
}

// Find label index
int get_label(char *label) {
    for (uint16_t i = 0; i < labHeight; i++) {
        if (strcmp(labStack[i].name, label)) return i;
    }
    return -1;
}

// Find variable index
int get_variable(char *variable) {
    for (uint16_t i = 0; i < varHeight; i++) {
        if (strcmp(varStack[i].name, variable)) return i;
    }
    return -1;
}

// Assemble code
void assemble(TOKEN *stack) {
    // Auxiliary variables
    int labelIndex;
    int nameIndex;
    uint64_t inAddr = 0;

    for (TOKEN *t = stack; t->tag != T_ENDPOINT; t++) {
        // Parse tokens
        switch (t->tag) {
            case T_NOTOKEN:
            case T_NEWLINE:
                break;
            case T_ENDPOINT:
                return;
            case T_LABEL:
                // Find label
                labelIndex = get_label((char *)t->content);
                // Move to next instruction
                inAddr++;

                if (labelIndex == -1)
                    push_label((NAMED_ADDR){(char *)t->content, inAddr});
                else
                    report_error_coord(E_DUPLICATED_LABEL, t->start);
                break;
            case T_VARSIZE:
                // Must treat size first!

                // Get variable name
                t++;
                nameIndex = get_variable((char *)t->content);
                if (nameIndex == -1) {
                    push_variable((NAMED_ADDR){(char *)t->content, inAddr});
                    // Skip '='
                    t++;
                    // Get variable value
                    t++;
                    // Set value on memory
                    set_tryte(memory, inAddr * 2,
                              (__tryte_ret)uint16_to_tryte(NOOP));

                    // Convert value
                    set_tryte(memory, inAddr * 2 + 1,
                              (__tryte_ret)uint16_to_tryte(
                                  *((uint16_t *)t->content)));

                    // Skip equations (provisory)
                    do t++;
                    while (t->tag != T_NEWLINE);

                    // Move to next instruction
                    inAddr++;
                } else
                    report_error_coord(E_DUPLICATED_NAME, t->start);
                break;
            case T_NAME:
                // Check if variable exist
                nameIndex = get_variable((char *)t->content);
                if (nameIndex == -1) {
                    report_error_coord(E_UNKNOWN_NAME, t->start);
                    break;
                }

                // Skip '='
                t++;
                // Get variable value
                t++;

                // Set value on memory
                set_tryte(
                    memory, varStack[nameIndex].addr * 2 + 1,
                    (__tryte_ret)uint16_to_tryte(*((uint16_t *)t->content)));
        }
    }
    puts(show_tryte(memory, 0, 5));
    puts("\n");
}