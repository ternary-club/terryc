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
    // General variables
    bool isFirstToken = true;
    bool isInsideLabel = false;
    // Specific variables
    bool hasMonadic;
    bool validOperand;
    while(1) {
        if(peep().tag == T_ENDPOINT) break;

        // Remove whitespaces from the beginning of the instruction
        advance_blank();
        
        // Validate variable declaration
        t = parse_token();
        switch(t.tag) {
            // Error
            case T_NOTOKEN:
                report_error(E_UNKNOWN_TOKEN);
                continue;
            // Size of variable
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
                    || t.tag == T_NAME) {
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
            // Numbers
            case T_INTB3:
            case T_INT3:
            case T_INT10:
            case T_INT27:
                // Check precedent
                if(isFirstToken)  report_error(E_UNEXPECTED_VALUE);
                continue;
            // Assignable entities
            case T_REGISTER:
            case T_NAME:
                // Check precedent
                if(isFirstToken) {
                    // Push assignable entity
                    push(t);

                    // Check successor
                    advance_blank();
                    t = parse_token();
                    // If it's not assertion, continue
                    if(t.tag != T_ASSERTION) {
                        report_error(E_EXPECTED_ASSERTION);
                        continue;
                    }

                    // Push assertion
                    push(t);

                    // Parse equation
                    while(1) {
                        // Check successor
                        advance_blank();
                        t = parse_token();
                        if(t.tag == T_MONADIC) {
                            // Advance monadic operators
                            do {
                                // Push monadic
                                push(t);
                                next();
                                parse_token();
                            } while(t.tag == T_MONADIC);
                            advance_blank();
                        }

                        // If a valid value was used
                        if(t.tag == T_INT10
                        || t.tag == T_INT27
                        || t.tag == T_INT3
                        || t.tag == T_INTB3
                        || t.tag == T_NAME) {
                            // Push value
                            push(t);
                            validOperand = true;
                        } else {
                            // Pop monadic operators
                            while(peep().tag == T_MONADIC) pop();
                            // Exception if register or label was used on expression
                            if(t.tag == T_REGISTER || t.tag == T_LABEL)
                                report_error(t.tag == T_REGISTER ? E_READ_REGISTER : E_READ_LABEL);
                            else report_error(E_EXPECTED_OPERAND);
                            validOperand = false;
                        }
                        
                        // Check successor
                        advance_blank();
                        t = parse_token();
                        // If it's a diadic operator
                        if(t.tag == T_DIADIC
                        || t.tag == T_MULTIDIC) {
                            // Push operator
                            if(validOperand) push(t);
                            continue;
                        } else if(!is_separator()) {
                            // If it's something else
                            report_error(E_EXPECTED_OPERATOR);
                            // Pop value
                            pop();
                            // Pop monadic operators
                            while(peep().tag == T_MONADIC) pop();
                        }
                        break;
                    }
                }
                isFirstToken = false;
                continue;
            // Labels
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
            // New lines
            case T_NEWLINE:
                // Push new line
                push(t);
                isFirstToken = true;
                continue;
            // End point
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