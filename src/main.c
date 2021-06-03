#ifndef STD_TRYTE_H
#define STD_TRYTE_H
#include "std/tryte.h"
#endif

#ifndef STD_OP_H
#define STD_OP_H
#include "std/op.h"
#endif

#ifndef STD_STR_H
#define STD_STR_H
#include "std/str.h"
#endif

#define BUFFER_SIZE 256     // Has to fit in mian buffer iterator
#define OP_BUFFER_SIZE 16   // Has to fit in operation buffer iterator

int main(int argc, char const* argv[]) {
    intptr file = open("./exec.tru");
    if(file <= 0) return 1;
    
    // Main buffer
    char buffer[BUFFER_SIZE];
    // Main buffer iterator
    char *mI = buffer;
    // Operation buffer
    char opBuffer[OP_BUFFER_SIZE];
    // Operation buffer iterator
    char *oI = opBuffer;

    while(1) {
        read(file, buffer, BUFFER_SIZE);
    }
}