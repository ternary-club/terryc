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

// Bad token flag enum
typedef enum {
    B_GOOD,
    B_BAD_FIRST,
    B_BAD,
} BAD_FLAG;

int main(int argc, char const *argv[]) {
    file = open("./lineTest.try");
    if(file <= 0) return 1;

    // Read buffer
    read(file, mBuffer, MAIN_BUFFER_SIZE);
    end = mBuffer;
    
    TOKEN t = new_token();
    // General variables
    BAD_FLAG badFlag = B_GOOD;
    bool isInsideLabel = false;
    // Specific variables
    bool hasMonadic;
    bool validOperand;
    while(1) {
        if(peep().tag == T_ENDPOINT) break;
        
        // Remove whitespaces from the beginning of the instruction
        advance_blank();
        
        // Validate variable declaration
        if(badFlag == B_BAD_FIRST)
            badFlag = B_BAD;
        else t = parse_token();
        // Switch tag
        switch(t.tag) {
            // Error
            case T_NOTOKEN:
                report_error(E_UNKNOWN_TOKEN);
                continue;
            // Operators
            case T_MONADIC:
            case T_DIADIC:
            case T_MULTIDIC:
            case T_LOGICAL:
                // Loose token error
                if(badFlag == B_BAD) {
                    report_error(E_UNEXPECTED_OPERATOR);
                    continue;
                }
                continue;
            case T_INTB3:
            case T_INT3:
            case T_INT10:
            case T_INT27:
                // Loose token error
                if(badFlag == B_BAD) {
                    report_error(E_UNEXPECTED_VALUE);
                    continue;
                }
                continue;
            case T_ASSERTION:
                // Loose token error
                if(badFlag == B_BAD) {
                    report_error(E_UNEXPECTED_ASSERTION);
                    continue;
                }
                continue;
            // Size of variable
            case T_VARSIZE:
                // Loose token error
                if(badFlag == B_BAD) {
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
                switch(t.tag) {
                    case T_NAME:
                        // Push name
                        push(t);
                        // Check successor
                        advance_blank();
                        t = parse_token();
                        switch(t.tag) {
                            case T_INT10:
                            case T_INT27:
                            case T_INT3:
                            case T_INTB3:
                            case T_NAME:
                                push(t);
                                break;
                            case T_LABEL:
                            case T_REGISTER:
                                // Exception if register or label was used on expression
                                report_error(t.tag == T_REGISTER ? E_READ_REGISTER : E_READ_LABEL);
                                // Pop name
                                pop();
                                // Pop size
                                pop();
                                break;
                            default:
                                // Pop name
                                pop();
                                // Pop size
                                pop();
                                report_error(E_EXPECTED_VALUE_VARDEC);
                        }
                        break;
                    default:
                        // Pop size
                        pop();
                        report_error(E_EXPECTED_NAME_VARDEC);
                }
                continue;
            // Assignable entities
            case T_REGISTER:
            case T_NAME:
                // Loose token error
                if(badFlag == B_BAD) {
                    report_error(t.tag == T_REGISTER ? E_UNEXPECTED_REGISTER : E_UNEXPECTED_NAME);
                    continue;
                }
                // Push assignable entity
                push(t);
                // Check successor
                advance_blank();
                t = parse_token();
                switch(t.tag) {
                    case T_ASSERTION:
                        // Push assertion
                        push(t);
                        // Parse equation
                        while(1) {
                            // Check successor
                            advance_blank();
                            t = parse_token();
                            // Parse monadic and multidic
                            switch(t.tag) {
                                case T_MONADIC:
                                case T_MULTIDIC:
                                    // Push current operator
                                    push(t);
                                    advance_blank();
                                    t = parse_token();
                                    // Advance operators
                                    while(t.tag == T_MONADIC || t.tag == T_MULTIDIC) {
                                        // Push operator
                                        push(t);
                                        next();
                                        t = parse_token();
                                    }
                                break;
                            }
                            // Parse number or variable (or other)
                            switch(t.tag) {
                                case T_INT10:
                                case T_INT27:
                                case T_INT3:
                                case T_INTB3:
                                case T_NAME:
                                    // Push value
                                    push(t);
                                    validOperand = true;
                                    // Next
                                    advance_blank();
                                    t = parse_token();
                                    break;
                                // If it's something else, abort
                                default:
                                    // Pop monadic operators
                                    while(peep().tag == T_MONADIC) pop();
                                    // Exception if register or label was used on expression
                                    if(t.tag == T_REGISTER || t.tag == T_LABEL)
                                        report_error(t.tag == T_REGISTER ? E_READ_REGISTER : E_READ_LABEL);
                                    else report_error(E_EXPECTED_OPERAND);
                                    // Set it's not a valid operand
                                    validOperand = false;
                            }
                            // If it's a diadic operator
                            if(t.tag != T_DIADIC
                            && t.tag != T_MULTIDIC) break;
                            // Push operator
                            else if(validOperand) push(t);
                        }
                        break;
                }
                // Push new line or end point
                switch(t.tag) {
                    case T_NEWLINE:
                    case T_ENDPOINT:
                        push(t);
                        continue;
                    default:
                        // Unexpected token error
                        badFlag = B_BAD_FIRST;
                }
                continue;
            // Labels
            case T_LABEL:
                // Loose token error
                if(badFlag == B_BAD) {
                    report_error(E_UNEXPECTED_LABEL);
                    continue;
                }
                // Push label
                push(t);
                // Check successor
                advance_blank();
                t = parse_token();
                switch(t.tag) {
                    case T_NEWLINE:
                    case T_ENDPOINT:
                        // Push new line or end point
                        push(t);
                        continue;
                    default:
                        // Unexpected token error
                        badFlag = B_BAD_FIRST;
                }
                continue;
            // New lines
            case T_NEWLINE:
                // Push new line
                push(t);
                badFlag = B_GOOD;
                continue;
            // End point
            case T_ENDPOINT:
                // Push end point
                push(t);
                break;
            default:
                report_error(E_UNKNOWN_TOKEN);
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
        }
        if(stack[i].tag != T_NEWLINE) puts(" ");
    }
    puts("\n");
}