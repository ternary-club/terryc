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

    // Read buffer
    read(file, mBuffer, MAIN_BUFFER_SIZE);
    end = mBuffer;
    
    TOKEN t = new_token();
    bool isFirstToken = true;
    bool isInsideLabel = false;
    bool hasMonadic;
    while(1) {
        if(peep().tag == T_ENDPOINT) break;

        // Remove whitespaces from the beginning of the instruction
        advance_blank();
        
        // Validate variable declaration
        t = parse_token();
        switch(t.tag) {
            case T_ASSERTION:
                hasMonadic = false;
                // Check precedent
                if(peep().tag != T_REGISTER && peep().tag != T_NAME) {
                    report_error(E_UNEXPECTED_ASSERTION);
                    continue;
                }

                // Push assertion
                push(t);

                // Check successor
                advance_blank();
                t = parse_token();
                if(t.tag == T_MONADIC) {
                    hasMonadic = true;
                    // Advance monadic operators
                    do {
                        // Push monadic
                        push(t);
                        parse_token();
                    } while(t.tag == T_MONADIC);
                }

                // Exception if register was used on expression
                if(t.tag == T_REGISTER) {
                    // Check if any monadic operators were used
                    if(peep().tag == T_MONADIC)
                        do pop(); while(peep().tag == T_MONADIC);
                    report_error(E_READ_REGISTER);
                }

                if(t.tag == T_LABEL) {
                    // Check if any monadic operators were used
                    if(peep().tag == T_MONADIC) {
                        do pop(); while(peep().tag == T_MONADIC);
                        report_error(E_OPERAND_LABEL);
                    }
                }

                // If a valid value was used
                if(t.tag == T_INT10
                || t.tag == T_INT27
                || t.tag == T_INT3
                || t.tag == T_INTB3
                || t.tag == T_NAME
                || t.tag == T_LABEL) {
                    // Exception for labels with monadic operators
                    if(t.tag == T_LABEL && peep().tag == T_MONADIC) {
                        do pop(); while(peep().tag == T_MONADIC);
                        report_error(E_OPERAND_LABEL);
                    }

                }

            case T_VARSIZE:
                // Check precedent
                if(!isFirstToken) {
                    report_error(E_UNEXPECTED_VARSIZE);
                    continue;
                }
                // Check if it's inside a label
                if(isInsideLabel) report_error(E_VARDEC_INSIDE_LABEL);

                // Push size
                push(t);

                // Check successor
                advance_blank();
                t = parse_token();
                if(t.tag == T_NAME) {
                    // Push name
                    push(t);

                    // Check successor
                    advance_blank();
                    t = parse_token();
                    if(t.tag == T_INT10
                    || t.tag == T_INT27
                    || t.tag == T_INT3
                    || t.tag == T_INTB3
                    || t.tag == T_NAME
                    || t.tag == T_LABEL) {
                        // Push value
                        push(t);
                    } else {
                        // Pop name
                        pop();
                        // Pop size
                        pop();
                        report_error(E_EXPECTED_VALUE_VARDEC);
                    }
                } else {
                    // Pop size
                    pop();
                    report_error(E_EXPECTED_NAME_VARDEC);
                }
                isFirstToken = false;
                continue;
            case T_INTB3:
            case T_INT3:
            case T_INT10:
            case T_INT27:
                // Check precedent
                if(isFirstToken)  report_error(E_UNEXPECTED_VALUE);
                continue;
            case T_REGISTER:
                // Check precedent
                if(!isFirstToken) {
                    report_error(E_UNEXPECTED_REGISTER);
                    continue;
                }
                // Push register
                push(t);
                // Check successor
                advance_blank();
                t = parse_token();
                if(t.tag != T_ASSERTION) {
                    report_error(E_EXPECTED_ASSERTION);
                    // Pop name
                    pop();
                }
                isFirstToken = false;
                continue;
            case T_NAME:
                // Check precedent
                if(!isFirstToken) {
                    report_error(E_UNEXPECTED_NAME);
                    continue;
                }
                // Push name
                push(t);
                // Check successor
                advance_blank();
                t = parse_token();
                if(t.tag != T_ASSERTION) {
                    report_error(E_EXPECTED_ASSERTION);
                    // Pop name
                    pop();
                }
                isFirstToken = false;
                continue;
            case T_LABEL:
                // Push label
                push(t);
                // Check precedent
                if(isFirstToken) {
                    // Check successor
                    advance_blank();
                    t = parse_token();
                    if(t.tag == T_NEWLINE) {
                        // Push new line
                        push(t);
                        isInsideLabel = true;
                        continue;
                    }
                }
                isFirstToken = false;
                continue;
            case T_NEWLINE:
                // Push new line
                push(t);
                isFirstToken = true;
                continue;
            case T_ENDPOINT:
                // Push end point
                push(t);
                isFirstToken = false;
                continue;
        }
    }

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
        }
        if(stack[i].tag != T_NEWLINE) puts(" ");
    }
    puts("\n");
}