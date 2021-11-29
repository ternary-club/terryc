typedef enum {
    NOOP = 0,
    COMP,
    JUMP,
    JMPG,
    JMPL,
    JMPGE,
    JMPLE,
    JMPE,
    JMPN,
    STORE,
    LOAD,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    CALL,
    HALT,
} OPERATION;

typedef struct {
    OPERATION code;
    uint8_t content;
} INSTRUCTION;