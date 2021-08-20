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

// This function was isolated from the main parsing switch because many
// structures accept equations and too many switch cases being executed
// in a pipeline changes the code architecture in an unnatural way.
// e.g.:
// case A:
//  foo();
// case B:
//  if(B) foo();
// case C:
//  if(C) foo();
// ...
void parse_equation(TOKEN *t) {
    // Parse equation
    while(1) {
        // Check successor
        advance_blank();
        *t = parse_token();
        // Parse monadic and multidic
        switch(t->tag) {
            case T_MONADIC:
            case T_MULTIDIC:
                // Push current operator
                push(*t);
                advance_blank();
                *t = parse_token();
                // Advance operators
                while(t->tag == T_MONADIC || t->tag == T_MULTIDIC) {
                    // Push operator
                    push(*t);
                    next();
                    *t = parse_token();
                }
            break;
        }
        bool validOperand;
        // Parse number or variable (or other)
        switch(t->tag) {
            case T_INT10:
            case T_INT27:
            case T_INT3:
            case T_INTB3:
            case T_NAME:
                // Push value
                push(*t);
                validOperand = true;
                // Next
                advance_blank();
                *t = parse_token();
                break;
            // If it's something else, abort
            default:
                // Pop monadic operators
                while(peep().tag == T_MONADIC) pop();
                // Exception if register or label was used on expression
                if(t->tag == T_REGISTER || t->tag == T_LABEL)
                    report_error(t->tag == T_REGISTER ? E_READ_REGISTER : E_READ_LABEL);
                else report_error(E_EXPECTED_OPERAND);
                // Set it's not a valid operand
                validOperand = false;
        }
        // If it's a diadic operator
        if(t->tag != T_DIADIC
        && t->tag != T_MULTIDIC) break;
        // Push operator
        else if(validOperand) push(*t);
    }
}

int main(int argc, char const *argv[]) {
    file = open("./test.try");
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
        if(badFlag == B_BAD_FIRST) badFlag = B_BAD;
        else t = parse_token();

        // Switch tag (always the first tag of instruction)
        switch(t.tag) {
            // Unknown token
            case T_NOTOKEN:
                report_error(E_UNKNOWN_TOKEN);
                continue;
            // Loose operators
            case T_MONADIC:
            case T_DIADIC:
            case T_MULTIDIC:
            case T_LOGICAL:
                continue;
            // Loose numbers
            case T_INTB3:
            case T_INT3:
            case T_INT10:
            case T_INT27:
                continue;
            // Loose assertion
            case T_ASSERTION:
                // If it's just a loose assertion, it's probably an invalid
                // variable assertion, so throw error
                report_error(E_UNTARGETED_ASSERTION);
                // And try to fix it by adding a dummy variable name
                TOKEN tName = new_token();
                tName.tag = T_NAME;
                // Push variable name
                push(tName);
                // Push assertion
                push(t);
                // Parse equation
                parse_equation(&t);
            // Variable size
            case T_VARSIZE:
                // If the instruction begins with a variable size, it's probably
                // a variable declaration.

                // Push variable size
                push(t);
                // Check successor
                advance_blank();
                t = parse_token();
                // Check if next token is a variable name
                if(t.tag != T_NAME) {
                    // If it's not, throw error
                    report_error(E_EXPECTED_NAME_VARDEC);
                    // Try to fix it by adding a dummy variable name
                    TOKEN tName = new_token();
                    tName.tag = T_NAME;
                    push(tName);
                }
                // Check if next token is an assertion
                if(t.tag != T_ASSERTION) {
                    // If it's not, throw error
                    report_error(E_EXPECTED_NAME_VARDEC);
                    // Try to fix it by adding a dummy variable name
                    TOKEN tName = new_token();
                    tName.tag = T_NAME;
                    push(tName);
                }
                // Parse equation
                parse_equation(&t);
                // Check if it's inside a label
                if(isInsideLabel) report_error(E_VARDEC_INSIDE_LABEL);
            case T_REGISTER:
            case T_NAME:
                // Loose token error
                if(badFlag == B_BAD) {
                    report_error(t.tag == T_REGISTER ? E_UNEXPECTED_REGISTER : E_UNEXPECTED_NAME);
                    continue;
                }
                // If it's an assignable entity, push it and check it's successor.
                // This checking is necessary because other switch cases may end up here.
                if(t.tag == T_REGISTER || t.tag == T_NAME) {
                    push(t);
                    // Check successor
                    advance_blank();
                    t = parse_token();
                }
                
                // Check current token
                switch(t.tag) {
                    case T_ASSERTION:
                        // Push assertion
                        push(t);
                        // Parse equation
                        parse_equation(&t);
                        break;
                    default:
                        report_error(E_EXPECTED_ASSERTION);
                        // Pop name
                        pop();
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