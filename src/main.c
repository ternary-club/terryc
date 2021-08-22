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

#define INFINITE 0xFF

// Fetch succeeding token not skipping any characters.
void fetch_only(TOKEN *t) {
    *t = parse_token();
}

// Skip whitespaces and fetch succeeding token.
void fetch(TOKEN *t) {
    while(is_empty()) next();
    fetch_only(t);
}

// Skip the rest of the line (instruction). Used to skip unexpected
// tokens that exceed any grammar structure and throw error if there
// are any exceeding tokens. Tolerates "offset" exceeding tokens
// before throwing an error
void forward(TOKEN *t, uint8_t offset) {
    // Check if instruction ends here
    if(t->tag == T_ENDPOINT || t->tag == T_NEWLINE) return;
    // The amount of fetched tokens
    uint8_t tokenCount = 0;
    // Until it's a new line or an end point
    while(t->tag != T_NEWLINE && t->tag != T_ENDPOINT) {
        // Report error on "offset" token
        if(offset != INFINITE && tokenCount++ == offset) report_error(E_EXPECTED_END);
        // Fetch successor
        fetch(t);
    }
    // Push ending token
    push(*t);
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
// break;
bool parse_equation(TOKEN *t) {
    // Dummy variable
    TOKEN tName;
    // If the equation didn't find strange tokens until the end
    bool succeeded = true;
    // Parse equation
    while(1) {
        // Parse monadic and multidic
        if(t->tag == T_MONADIC || t->tag == T_MULTIDIC) {
            // Push current operator
            push(*t);
            fetch(t);
            // Advance operators
            while(t->tag == T_MONADIC || t->tag == T_MULTIDIC) {
                // Push operator
                push(*t);
                next();
                fetch_only(t);
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
                fetch(t);
                break;
            // If it's something else, skip
            default:
                // Try to fix it by pushing a dummy variable
                tName = NEW_TOKEN;
                tName.tag = T_NAME;
                push(tName);
                // Exception if register or label was used on expression
                if(t->tag == T_REGISTER || t->tag == T_LABEL)
                    report_error(t->tag == T_REGISTER ? E_READ_REGISTER : E_READ_LABEL);
                else {
                    // The parsing did find strange tokens
                    succeeded = false;
                    report_error(E_EXPECTED_OPERAND);
                }
                // If it's a connector operator, it's probably a missing operand, so don't ignore it.
                // If it's an ending token, it's probably a bad ending equation, so don't ignore it.
                // If it's anything else, it's probably a bad operand, so ignore it.
                if(t->tag != T_DIADIC && t->tag != T_MULTIDIC
                && t->tag != T_ENDPOINT && t->tag != T_NEWLINE) fetch(t);
        }

        // Check if there's a diadic or multidic operator to connect expressions
        switch(t->tag) {
            case T_MULTIDIC:
            case T_DIADIC:
                // Push operator
                push(*t);
                // Fetch successor
                fetch(t);
                continue;
            case T_ENDPOINT:
            case T_NEWLINE:
                // Push end
                push(*t);
            default:
                return succeeded;
        }
    }
}

int main(int argc, const char *argv[]) {
    // Open file
    file = open(argv[1]);
    if(file <= 0) return 1;

    // Set file path for error handling
    filename = argv[1];

    // Read buffer
    read(file, mBuffer, MAIN_BUFFER_SIZE);
    end = mBuffer;
    
    TOKEN t = NEW_TOKEN;
    // General variables
    bool isInsideLabel = false;
    // Specific variables
    bool hasMonadic;
    bool succeeded;
    uint8_t paramCount;
    while(1) {
        if(t.tag == T_ENDPOINT) break;
        
        // Fetch successor
        fetch(&t);

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
                // Skip every token after this one
                forward(&t, INFINITE);
                continue;
            // Loose numbers
            // e.g.: $ 5
            case T_INTB3:
            case T_INT3:
            case T_INT10:
            case T_INT27:
                // Throw error
                report_error(E_UNEXPECTED_NUMBER);
                // Skip every token after this one
                forward(&t, INFINITE);
                continue;
            // Loose assertion
            // e.g.: $ =
            case T_ASSERTION:
                // If it's just a loose assertion, it's probably an invalid
                // variable assertion, so throw error
                report_error(E_UNTARGETED_ASSERTION);
                // And try to fix it by pushing a dummy variable name
                // e.g.: $ foo =
                TOKEN tName = NEW_TOKEN;
                tName.tag = T_NAME;
                // Push variable name
                push(tName);
                // Push assertion
                push(t);
                // Fetch successor
                fetch(&t);
                // Parse equation
                // e.g.: $ foo = 2 + 5
                succeeded = parse_equation(&t);
                // Skip every token after this one
                forward(&t, (uint8_t)!succeeded);
                continue;
            // Command
            // e.g.: $ goto
            case T_COMMAND:
                // Parameter count for error handling
                paramCount = 0;
                // Push command
                push(t);
                // Check if it's inside a label
                if(!isInsideLabel) report_error(E_COMMAND_OUTSIDE_LABEL);
                switch(*((uint8_t*)t.content)) {
                    // e.g.: $ store
                    case C_STORE:
                        // Fetch successor
                        fetch(&t);
                        // Parse first argument (value) and count legitimate parameter
                        // e.g.: $ store 8
                        if(parse_equation(&t)) paramCount++;
                        // Parse second argument (register)
                        // e.g.: $ store 8 1a
                        if(t.tag == T_REGISTER) {
                            // Push register
                            push(t);
                            // Fetch successor
                            fetch(&t);
                            // Count legitimate parameter
                            paramCount++;
                        } else {
                            // Throw error
                            report_error(E_EXPECTED_REGISTER);
                            // Try to fix it by pushing an assertion
                            TOKEN tRegister = NEW_TOKEN;
                            tRegister.tag = T_REGISTER;
                            push(tRegister);
                        }
                        // Skip every token after this one
                        forward(&t, 2 - paramCount);
                        break;
                    // e.g.: $ load
                    case C_LOAD:
                        // Fetch successor
                        fetch(&t);
                        // Parse first argument (register)
                        // e.g.: $ load 3b
                        if(t.tag == T_REGISTER) {
                            // Push register
                            push(t);
                            // Fetch successor
                            fetch(&t);
                            // Count legitimate parameter
                            paramCount++;
                        } else {
                            // Throw error
                            report_error(E_EXPECTED_REGISTER);
                            // Try to fix it by pushing an assertion
                            TOKEN tRegister = NEW_TOKEN;
                            tRegister.tag = T_REGISTER;
                            push(tRegister);
                        }
                        // Parse second argument (value) and count legitimate parameter
                        // e.g.: $ load 3b foo
                        if(parse_equation(&t)) paramCount++;
                        // Skip every token after this one
                        forward(&t, 2 - paramCount);
                        break;
                    // e.g.: $ call
                    case C_CALL:
                        // Fetch successor
                        fetch(&t);
                        // Parse argument (value) and count legitimate parameter
                        // e.g.: $ call 1
                        if(parse_equation(&t)) paramCount++;
                        // Skip every token after this one
                        forward(&t, 1 - paramCount);
                        break;
                    // e.g.: $ goto
                    case C_GOTO:
                        // Fetch successor
                        fetch(&t);
                        // Parse argument (label)
                        // e.g.: $ goto 3b
                        if(t.tag == T_LABEL) {
                            // Push label
                            push(t);
                            // Fetch successor
                            fetch(&t);
                            // Count legitimate parameter
                            paramCount++;
                        } else {
                            // Throw error
                            report_error(E_EXPECTED_LABEL);
                            // Try to fix it by pushing an assertion
                            TOKEN tLabel = NEW_TOKEN;
                            tLabel.tag = T_LABEL;
                            push(tLabel);
                        }
                        // Skip every token after this one
                        forward(&t, 1 - paramCount);
                        break;
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
                fetch(&t);
                // Check if it has a variable name 
                // e.g.: $ tryte foo
                if(t.tag == T_NAME) {
                    // Push name
                    push(t);
                    // Fetch successor
                    fetch(&t);
                } else {
                    // Throw error
                    report_error(E_EXPECTED_NAME_VARDEC);
                    // Try to fix it by pushing a dummy variable name
                    TOKEN tName = NEW_TOKEN;
                    tName.tag = T_NAME;
                    push(tName);
                }
               
                // Check if it has an assertion
                // e.g.: $ tryte foo =
                if(t.tag == T_ASSERTION) {
                    // Push assertion
                    push(t);
                    // Fetch successor
                    fetch(&t);
                } else {
                    // Throw error
                    report_error(E_EXPECTED_ASSERTION);
                    // Try to fix it by pushing an assertion
                    TOKEN tAssertion = NEW_TOKEN;
                    tAssertion.tag = T_ASSERTION;
                    push(tAssertion);
                }

                // Parse equation
                succeeded = parse_equation(&t);
                // Skip every token after this one
                forward(&t, (uint8_t)!succeeded);
                continue;
            // Assignable entity
            // e.g.: $ 1a
            // e.g.: $ foo
            case T_REGISTER:
            case T_NAME:
                // Push assignable entity
                push(t);
                // Fetch successor
                fetch(&t);

                // Check if it has an assertion
                // e.g.: $ 1a =
                // e.g.: $ foo =
                if(t.tag == T_ASSERTION) {
                    // Push assertion
                    push(t);
                    // Fetch successor
                    fetch(&t);
                } else {
                    // Throw error
                    report_error(E_EXPECTED_ASSERTION);
                    // Try to fix it by pushing an assertion
                    TOKEN tAssertion = NEW_TOKEN;
                    tAssertion.tag = T_ASSERTION;
                    push(tAssertion);
                }

                // Parse equation
                succeeded = parse_equation(&t);
                // Skip every token after this one
                forward(&t, (uint8_t)!succeeded);
                continue;
            // Labels
            // e.g.: $ Foo
            case T_LABEL:
                // Mark inside of label
                isInsideLabel = true;
                // Push label
                push(t);
                // Fetch successor
                fetch(&t);
                // Skip every token after this one
                forward(&t, 0);
                continue;
            // New lines
            // e.g.: $ \n
            // e.g.: $ \r\n
            case T_NEWLINE:
                // Push new line
                push(t);
                // Skip multiple new lines
                do
                    // Fetch successor
                    fetch(&t);
                while(t.tag == T_NEWLINE);
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
            case T_COMMAND:
                puts("comm");
                break;
        }
        if(stack[i].tag != T_NEWLINE) puts(" ");
    }
    puts("\n");
}