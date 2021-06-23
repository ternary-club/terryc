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

#ifndef STD_BOOL_H
#define STD_BOOL_H
#include "std/bool.h"
#endif

#ifndef DRIVER_MEM_H
#define DRIVER_MEM_H
#include "driver/mem.h"
#endif

#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H
#include "compiler/parser.h"
#endif

#ifndef COMPILER_ERROR_H
#define COMPILER_ERROR_H
#include "compiler/errors.h"
#endif


int main(int argc, char const *argv[]) {

    file = open("./test.try");
    if(file <= 0) return - 1;

    bool isInsideLabel = false;

    // Read buffer
    read(file, mBuffer, MAIN_BUFFER_SIZE);
    end = mBuffer;
    
    while(1) {
        if(is_etx()) break;

        // Remove whitespaces from the beginning of the instruction
        advance_blank();
        
        // Validate variable declaration
        if(parse_size()) {
            // Check precedent
            if(*((TAG*)peep().content) != T_NOTOKEN && *((TAG*)peep().content) != T_NEWLINE) {
                report_error(E_UNEXPECTED_VARSIZE);
                continue;
            }

            // Check if it's inside a label
            if(isInsideLabel) report_error(E_VARDEC_INSIDE_LABEL);

            // Check successor
            advance_blank();
            if(parse_var()) {
                advance_blank();
                if(!parse_value()) report_error(E_EXPECTED_VALUE);
            }
            else report_error(E_EXPECTED_NAME);
        }

        // Check if it's \n
        parse_newline();
    }

    for(uint64_t i = 0; i < height; i++) {
        puts(stack[i].tag == T_NEWLINE ? "\n" : itoa(stack[i].tag));
    }
}