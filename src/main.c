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
    if(file <= 0) return 1;

    bool isInsideLabel = false;

    // Read buffer
    read(file, mBuffer, MAIN_BUFFER_SIZE);
    end = mBuffer;
    
    TOKEN t = new_token();
    while(t.tag != T_ENDPOINT) {
        advance_blank();
        t = parse_token();
        push(t);
    }

    // while(1) {
    //     if(is_etx()) break;

    //     // Remove whitespaces from the beginning of the instruction
    //     advance_blank();
        
    //     // Validate variable declaration
    //     if(parse_token().tag == T_VARSIZE) {
    //         // Check precedent
    //         if(*((TAG*)peep().content) != T_NOTOKEN && *((TAG*)peep().content) != T_NEWLINE) {
    //             report_error(E_UNEXPECTED_VARSIZE);
    //             continue;
    //         }

    //         // Check if it's inside a label
    //         if(isInsideLabel) report_error(E_VARDEC_INSIDE_LABEL);

    //         // Check successor
    //         advance_blank();
    //         if(parse_var()) {
    //             advance_blank();
    //             if(!parse_value()) report_error(E_EXPECTED_VALUE);
    //         }
    //         else report_error(E_EXPECTED_NAME);
    //     }

    //     // Check if it's \n
    //     parse_newline();
    // }

    for(uint64_t i = 0; i < height; i++) {
        switch(stack[i].tag) {
            case T_NOTOKEN:
                puts("?");
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
            case T_ENDPOINT:
                puts("end");
                break;
            case T_COMMENT:
                puts("comment");
                break;
        }
        if(stack[i].tag != T_NEWLINE) puts(" ");
    }
    puts("\n");
}