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

    file = open("./teste");
    if(file <= 0) return 1;

    // Read buffer
    read(file, mBuffer, MAIN_BUFFER_SIZE);

    puts(mBuffer);

    // file = open("./exec.try");
    // if(file <= 0) return 1;

    // while(1) {
    //     // Read buffer
    //     read(file, mBuffer, MAIN_BUFFER_SIZE);
    //     mI = mBuffer;

    //     // Remove whitespaces from the beginning of the instruction
    //     skip_whitespaces();

    //     // Parse variable sizes
    //     parse_size();

    //     // Try to parse numbers
    //     parse_number();
    //     if(sI > sBuffer)
    //         // Try to parse base literals
    //         if(is_letter() && sI[0] == '0' && sI == sBuffer - 1)
    //             switch(*mI) {
    //                 case 'h':
    //                     parse_heptavintimal_number();
    //                     if(sI > sBuffer) {
    //                         //add_token(TOKEN_INT27);
    //                         continue;
    //                     }
    //                     break;
    //                 case 't':
    //                     parse_ternary_number();
    //                     if(sI > sBuffer) {
    //                         //add_token(TOKEN_INT3);
    //                         continue;
    //                     }
    //                     break;
    //                 case 'b':
    //                     parse_balanced_ternary_number();
    //                     if(sI > sBuffer) {
    //                         //add_token(TOKEN_INTB3);
    //                         continue;
    //                     }
    //                     break;
    //                 default:
    //                     report_error(ERR_INVALID_BASE_LITERAL);
    //             }
    //         else {
    //             //add_token(TOKEN_INT10);
    //             continue;
    //         }
    // }
}