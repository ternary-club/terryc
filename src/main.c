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

#ifndef STD_TYPES_H
#define STD_TYPES_H
#include "std/types.h"
#endif

#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H
#include "compiler/parser.h"
#endif

#define BUFFER_SIZE 256     // Has to fit in main buffer iterator
#define OP_BUFFER_SIZE 16   // Has to fit in operation buffer iterator

int main(int argc, char const* argv[]) {
    intptr file = open("./exec.try");
    if(file <= 0) return 1;
    
    // Main buffer
    char mBuffer[BUFFER_SIZE];
    // Main buffer iterator
    char *mI;
    // Operation buffer
    char oBuffer[OP_BUFFER_SIZE];
    // Operation buffer iterator
    char *oI = oBuffer;

    while(1) {
        // Read buffer
        read(file, mBuffer, BUFFER_SIZE);
        mI = mBuffer;
        
        // Skip whitespaces or tabs
        while(is_empty(*mI)) {
            // Next char on buffer
            mI++;
            // If it has reached the end of buffer, load next chars
            if(mI > mBuffer + BUFFER_SIZE) {
                read(file, mBuffer, BUFFER_SIZE);
                mI = mBuffer;
            }
        }

        // Try to detect instruction
        if(is_char(*mI)) {
            // Reset operation iterator
            oI = oBuffer;
            // Detect characters
            while(is_char(*mI)) {
                // If there's still space, add char to the operation buffer
                if(oI <= oBuffer + OP_BUFFER_SIZE) oBuffer[*oI++] = *mI;
                // Next char
                mI++;
                // If it has reached the end of the buffer, load next chars
                if(mI > mBuffer + BUFFER_SIZE) {
                    read(file, mBuffer, BUFFER_SIZE);
                    mI = mBuffer;
                }
            }
        }
    }
}