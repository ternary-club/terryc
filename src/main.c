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

// Skip the rest of the line (instruction). Used to skip unexpected
// tokens that exceed any grammar structure. Be sure to throw the error
// before calling this.
void skip_line(TOKEN *t) {
    // Until it's a new line or an end point
    while(t->tag != T_NEWLINE && t->tag != T_ENDPOINT) {
        // Fetch successor
        advance_blank();
        *t = parse_token();
    }
}

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
    // Dummy variable
    TOKEN tName;
    // Parse equation
    while(1) {
        // Parse monadic and multidic
        if(t->tag == T_MONADIC || t->tag == T_MULTIDIC) {
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
        }

        // Parse number or variable (or other)
        switch(t->tag) {
            case T_INT10:
            case T_INT27:
            case T_INT3:
            case T_INTB3:
            case T_NAME:
                // Push value
                push(*t);
                // Fetch successor
                advance_blank();
                *t = parse_token();
                break;
            // If it's something else, skip
            default:
                // Try to fix it by pushing a dummy variable
                tName = new_token();
                tName.tag = T_NAME;
                push(tName);
                // Exception if register or label was used on expression
                if(t->tag == T_REGISTER || t->tag == T_LABEL)
                    report_error(t->tag == T_REGISTER ? E_READ_REGISTER : E_READ_LABEL);
                else report_error(E_EXPECTED_OPERAND);
                // If isn't a diadic or multidic operator, ignore current token
                if(t->tag != T_DIADIC && t->tag != T_MULTIDIC) {
                    // Fetch successor
                    advance_blank();
                    *t = parse_token();
                }
        }

        // Check if there's a diadic or multidic operator to connect expressions
        switch(t->tag) {
            case T_MULTIDIC:
            case T_DIADIC:
                // Push operator
                push(*t);
                // Fetch successor
                advance_blank();
                *t = parse_token();
                continue;
            case T_ENDPOINT:
            case T_NEWLINE:
                // Push end
                push(*t);
            default:
                return;
        }
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
    bool isInsideLabel = false;
    // Specific variables
    bool hasMonadic;
    while(1) {
        if(peep().tag == T_ENDPOINT) break;
        
        // Remove whitespaces from the beginning of the instruction
        advance_blank();
        // Validate variable declaration
        t = parse_token();

        // Switch tag (always the first tag of instruction)
        switch(t.tag) {
            // Loose operators
            // e.g.: $ +
            case T_MONADIC:
            case T_DIADIC:
            case T_MULTIDIC:
            case T_LOGICAL:
                // Throw error
                report_error(E_UNEXPECTED_OPERATOR);
                continue;
            // Loose numbers
            // e.g.: $ 5
            case T_INTB3:
            case T_INT3:
            case T_INT10:
            case T_INT27:
                // Throw error
                report_error(E_UNEXPECTED_NUMBER);
                continue;
            // Loose assertion
            // e.g.: $ =
            case T_ASSERTION:
                // If it's just a loose assertion, it's probably an invalid
                // variable assertion, so throw error
                report_error(E_UNTARGETED_ASSERTION);
                // And try to fix it by pushing a dummy variable name
                // e.g.: $ foo =
                TOKEN tName = new_token();
                tName.tag = T_NAME;
                // Push variable name
                push(tName);
                // Push assertion
                push(t);
                // Fetch successor
                advance_blank();
                t = parse_token();
                // Parse equation
                // e.g.: $ foo = 2 + 5
                parse_equation(&t);
                // If it doesn't end there, throw error
                if(t.tag != T_ENDPOINT && t.tag != T_NEWLINE) {
                    report_error(E_EXPECTED_END);
                    // Skip every token after this one
                    skip_line(&t);
                }
                continue;
            // Variable size
            // e.g.: $ tryte
            case T_VARSIZE:
                // Push variable size
                push(t);
                // Check if it's inside a label
                if(isInsideLabel) report_error(E_VARDEC_INSIDE_LABEL);
                // Fetch successor
                advance_blank();
                t = parse_token();
                // Check if it has a variable name 
                // e.g.: $ tryte foo
                if(t.tag == T_NAME) {
                    // Push name
                    push(t);
                    // Fetch successor
                    advance_blank();
                    t = parse_token();
                } else {
                    // Throw error
                    report_error(E_EXPECTED_NAME_VARDEC);
                    // Try to fix it by pushing a dummy variable name
                    TOKEN tName = new_token();
                    tName.tag = T_NAME;
                    push(tName);
                }
               
                // Check if it has an assertion
                // e.g.: $ tryte foo =
                if(t.tag == T_ASSERTION) {
                    // Push assertion
                    push(t);
                    // Fetch successor
                    advance_blank();
                    t = parse_token();
                } else {
                    // Throw error
                    report_error(E_EXPECTED_ASSERTION);
                    // Try to fix it by pushing an assertion
                    TOKEN tAssertion = new_token();
                    tAssertion.tag = T_ASSERTION;
                    push(tAssertion);
                }

                // Parse equation
                parse_equation(&t);
                // If it doesn't end there, throw error
                if(t.tag != T_ENDPOINT && t.tag != T_NEWLINE) {
                    report_error(E_EXPECTED_END);
                    // Skip every token after this one
                    skip_line(&t);
                }
                continue;
            // Assignable entity
            // e.g.: $ 1a
            // e.g.: $ foo
            case T_REGISTER:
            case T_NAME:
                // Push assignable entity
                push(t);
                // Fetch successor
                advance_blank();
                t = parse_token();

                // Check if it has an assertion
                // e.g.: $ 1a =
                // e.g.: $ foo =
                if(t.tag == T_ASSERTION) {
                    // Push assertion
                    push(t);
                    // Fetch successor
                    advance_blank();
                    t = parse_token();
                } else {
                    // Throw error
                    report_error(E_EXPECTED_ASSERTION);
                    // Try to fix it by pushing an assertion
                    TOKEN tAssertion = new_token();
                    tAssertion.tag = T_ASSERTION;
                    push(tAssertion);
                }

                // Parse equation
                parse_equation(&t);
                // If it doesn't end there, throw error
                if(t.tag != T_ENDPOINT && t.tag != T_NEWLINE) {
                    report_error(E_EXPECTED_END);
                    // Skip every token after this one
                    skip_line(&t);
                }
                continue;
            // Labels
            // e.g.: $ Foo
            case T_LABEL:
                // Push label
                push(t);
                // Fetch successor
                advance_blank();
                t = parse_token();
                // If it doesn't end there, throw error
                if(t.tag != T_ENDPOINT && t.tag != T_NEWLINE) {
                    report_error(E_EXPECTED_END);
                    // Skip every token after this one
                    skip_line(&t);
                }
                // Push instruction end
                push(t);
                continue;
            // New lines
            // e.g.: $ \n
            // e.g.: $ \r\n
            case T_NEWLINE:
                // Push new line
                push(t);
                // Skip multiple new lines
                do {
                    // Fetch successor
                    advance_blank();
                    t = parse_token();
                } while(t.tag == T_NEWLINE);
                continue;
            // End point
            // e.g.: $ \0
            // e.g.: $ \x03
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