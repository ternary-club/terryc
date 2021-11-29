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
#include "compiler/error.h"
#endif

#define INFINITE 0xFF

void putt(TAG tag) {
    switch (tag) {
        default:
            puts("UNKNOWN");
            break;
        case T_ENDPOINT:
            puts(".");
            break;
        case T_NOTOKEN:
            puts("INVALID");
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
        case T_QUATERNARY:
            puts("?");
            break;
        case T_OUTCOME:
            puts("outcome");
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
    // puts("(");
    // puts(itoa(tag));
    // puts(")");
}

// Skip whitespaces and fetch succeeding token.
void fetch(TOKEN *t) {
    while (is_empty()) next();
    *t = parse_token();
}

// Skip the rest of the line (instruction). Used to skip unexpected
// tokens that exceed any grammar structure and throw error if there
// are any exceeding tokens. Tolerates "offset" exceeding tokens
// before throwing an error
void forward(TOKEN *t, uint8_t offset) {
    // The amount of fetched tokens
    uint8_t tokenCount = 0;
    // Until it's a new line or an end point
    while (t->tag != T_NEWLINE && t->tag != T_ENDPOINT) {
        // Report error on "offset" token
        if (offset != INFINITE && tokenCount++ == offset)
            report_error(E_EXPECTED_END);
        // Fetch successor
        fetch(t);
    }
    // Push ending token
    push(*t);
}

// Equation type enum
typedef enum {
    EQ_NONE = 0,
    EQ_VALUE,
    EQ_LABEL,
} EQUATION;

// This function was isolated from the main parsing switch because many
// structures accept equations and too many switch cases being executed
// in a pipeline change the code architecture in an unnatural way.
// e.g.:
// case A:
//  foo();
// case B:
//  if(B) foo();
// case C:
//  if(C) foo();
// break;
EQUATION parse_equation(TOKEN *t) {
    // ----------------------------------------------------
    // 'Global' variables (variables that really should be
    // outside the switch statement so they can be preserved
    // to the next iterations

    // Current type of equation
    EQUATION type = EQ_NONE;
    // If the last diadic operator (or multidic used as diadic)
    // was logical
    bool afterLogicalOperation = false;

    // ----------------------------------------------------
    // Auxiliary variables (due to C not letting us declare
    // variables inside switch statements)

    // Dummy variable used for error correction
    TOKEN tDummy;
    // Quaternary operator equations return type
    EQUATION eq1;
    EQUATION eq2;
    EQUATION eq3;
    // Quaternary operator final return type
    EQUATION ret;
    // If quaternary operator will use a third equation
    bool has3rdEquation;
    // Quaternary operator outcomes
    OUTCOME o1;
    OUTCOME o2;

    // ----------------------------------------------------
    // Equation parsing

    while (1) {
        // Parse monadic and multidic
        while (t->tag == T_MONADIC || t->tag == T_MULTIDIC) {
            // Set equation type if still not defined (or throw error
            // if it dissonates from current type)
            if (!type) type = EQ_VALUE;
            if (type != EQ_VALUE)
                report_error(E_READ_OPERATOR);
            else
                // Push operator
                push(*t);
            // Fetch successor
            fetch(t);
        }

        // Parse number, variable, quaternary operator (or other)
        switch (t->tag) {
            case T_INT10:
            case T_INT27:
            case T_INT3:
            case T_INTB3:
            case T_NAME:
            case T_REGISTER:
                // Set equation type if still not defined (or throw
                // error if it dissonates from current type)
                if (!type) type = EQ_VALUE;
                if (type != EQ_VALUE) {
                    // Try to fix it by pushing a dummy token
                    tDummy = NEW_TOKEN;
                    switch (type) {
                        case EQ_LABEL:
                            tDummy.tag = T_LABEL;
                            break;
                    }
                    push(tDummy);
                    report_error(E_READ_NUMBER);
                } else
                    // Push number
                    push(*t);
                // Fetch successor
                fetch(t);
                break;
            case T_LABEL:
                // Set equation type if still not defined (or throw error
                // if it dissonates from current type)
                if (!type) type = EQ_LABEL;
                if (type != EQ_LABEL) {
                    // Try to fix it by pushing a dummy token
                    tDummy = NEW_TOKEN;
                    switch (type) {
                        case EQ_VALUE:
                            tDummy.tag = T_NAME;
                            break;
                    }
                    push(tDummy);
                    report_error(E_READ_LABEL);
                } else
                    // Push label
                    push(*t);
                // Fetch successor
                fetch(t);
                break;
            // If it's something else, don't push it
            default:
                // If it's finished, break
                if (t->tag == T_ENDPOINT || t->tag == T_NEWLINE)
                    report_error(E_EXPECTED_VALUE);
                else {
                    // Try to fix it by pushing a dummy token
                    tDummy = NEW_TOKEN;
                    switch (type) {
                        case EQ_VALUE:
                            tDummy.tag = T_NAME;
                            break;
                        case EQ_LABEL:
                            tDummy.tag = T_LABEL;
                            break;
                    }
                    if (type != EQ_NONE) {
                        push(tDummy);
                        report_error(E_EXPECTED_OPERAND);
                    } else if (t->tag != T_ENDPOINT && t->tag != T_NEWLINE)
                        report_error(E_UNKNOWN_TOKEN);
                }
        }

        // Check if it begins a quaternary operation
        if (t->tag == T_QUATERNARY) {
            // If quaternary operator doesn't come right after a
            // logical equation, throw error
            if (!afterLogicalOperation)
                report_error(E_UNEXPECTED_QUATERNARY_OPERATOR);

            // Connector operator is not logical (it is a quaternary
            // operator)
            afterLogicalOperation = false;
            // Reset equation return types
            eq1 = EQ_NONE;
            eq2 = EQ_NONE;
            eq3 = EQ_NONE;
            // Reset third equation checking
            has3rdEquation = false;

            // Push quaternary operator
            push(*t);
            // Fetch successor
            fetch(t);

            // Ignore new lines
            while (t->tag == T_NEWLINE) fetch(t);

            // Check the FIRST outcome
            if (t->tag != T_OUTCOME) {
                // Try to fix it by pushing a dummy outcome
                tDummy = NEW_TOKEN;
                tDummy.tag = T_OUTCOME;
                *((uint8_t *)tDummy.content) = o1 = O_IGNORE;
                push(tDummy);
                report_error(E_EXPECTED_OUTCOME);
            } else {
                // Fix outcome type if needed
                if (*((uint8_t *)t->content) == O_ELSE) {
                    report_error(E_UNEXPECTED_ELSE_OUTCOME);
                    *((uint8_t *)t->content) = O_IGNORE;
                }

                // Record outcome for error purposes
                o1 = *((uint8_t *)t->content);
                // Push outcome
                push(*t);
                // Fetch successor
                fetch(t);
            }

            // Parse FIRST equation
            eq1 = parse_equation(t);

            // Ignore new lines
            while (t->tag == T_NEWLINE) fetch(t);

            // Check the SECOND outcome
            if (t->tag != T_OUTCOME) {
                // Try to fix it by pushing a dummy outcome
                tDummy = NEW_TOKEN;
                tDummy.tag = T_OUTCOME;
                *((uint8_t *)tDummy.content) = o2 = O_IGNORE;
                push(tDummy);
                report_error(E_EXPECTED_OUTCOME);
            } else {
                // Check for repeated outcomes
                if (*((uint8_t *)t->content) & o1 && o1 != O_IGNORE) {
                    report_error(E_MULTIPLE_OUTCOME);
                    *((uint8_t *)t->content) = O_IGNORE;
                }

                // Record outcome for error purposes
                o2 = *((uint8_t *)t->content);
                // Push outcome
                push(*t);
                // Fetch successor
                fetch(t);
            }

            // Parse SECOND equation
            eq2 = parse_equation(t);

            // If there is still a route missing, parse third outcome
            if (o1 != O_IGNORE && o2 != O_IGNORE && o2 != O_ELSE &&
                (o1 | o2) != 7) {  // 7 = 0b111

                // Record use of a third equation
                has3rdEquation = true;

                // Ignore new lines
                while (t->tag == T_NEWLINE) fetch(t);

                // Check the THIRD outcome
                if (t->tag != T_OUTCOME) {
                    // Try to fix it by pushing a dummy outcome
                    tDummy = NEW_TOKEN;
                    tDummy.tag = T_OUTCOME;
                    *((uint8_t *)tDummy.content) = O_IGNORE;
                    push(tDummy);
                    report_error(E_EXPECTED_OUTCOME);
                } else {
                    // Check for repeated outcomes and finished quaternary
                    // operation
                    if (o2 == O_ELSE ||
                        (*((uint8_t *)t->content) & o1 && o1 != O_IGNORE) ||
                        (*((uint8_t *)t->content) & o2 && o2 != O_IGNORE)) {
                        report_error(E_MULTIPLE_OUTCOME);
                        *((uint8_t *)t->content) = O_IGNORE;
                    }

                    // Push outcome
                    push(*t);
                    // Fetch successor
                    fetch(t);
                }

                // Parse THIRD equation
                eq3 = parse_equation(t);
            }

            // Check if quaternary operator has multiple returns
            if (eq1 != eq2 || eq2 != eq3 && has3rdEquation) {
                ret = EQ_NONE;
                report_error(E_MULTIPLE_QUATERNARY_RETURN_TYPES);
            } else
                ret = eq1;

            // Set equation type if still not defined (or throw error
            // if it dissonates from current type)
            if (!type) type = ret;
            if (type != ret) report_error(E_INVALID_QUATERNARY_RETURN_TYPES);
        }

        // Check if there's a diadic, multidic or logical operator
        // to connect expressions
        if (t->tag == T_DIADIC || t->tag == T_MULTIDIC || t->tag == T_LOGICAL) {
            // Record if connector operator is logical
            afterLogicalOperation = t->tag == T_LOGICAL;
            // Set equation type if still not defined (or throw error
            // if it dissonates from current type)
            if (!type) type = EQ_VALUE;
            if (type != EQ_VALUE)
                report_error(E_READ_OPERATOR);
            else
                // Push operator
                push(*t);
            // Fetch successor
            fetch(t);
            continue;
        }
        return type;
    }
}

int main(int argc, const char *argv[]) {
    bool noCompile = false;
    // Iterate through arguments
    for (uint8_t i = 2; i < argc; i++)
        if (strcmp(argv[i], "-nocompile"))
            noCompile = true;
        else {
            report_warning("unknown flag", false);
            puts(" \"");
            puts(argv[i]);
            puts("\".\n");
        }

    // Open file
    file = open(argv[1]);
    if (file <= 0) return 1;

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
    while (1) {
        // If it hits the end, breaks
        if (t.tag == T_ENDPOINT) break;

        // Fetch successor
        fetch(&t);

        // Switch tag (always the first tag of instruction)
        switch (t.tag) {
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
                // Check if it's outside a label
                if (!isInsideLabel) report_error(E_ASSERTION_OUTSIDE_LABEL);
                // And try to fix it by pushing a dummy variable name
                // e.g.: $ foo =
                TOKEN tDummy = NEW_TOKEN;
                tDummy.tag = T_NAME;
                // Push variable name
                push(tDummy);
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
                if (!isInsideLabel) report_error(E_COMMAND_OUTSIDE_LABEL);
                switch (*((uint8_t *)t.content)) {
                        // e.g.: $ call
                    case C_CALL:
                        // Fetch successor
                        fetch(&t);
                        // Parse argument (value) and count legitimate
                        // parameter e.g.: $ call 1
                        if (parse_equation(&t)) paramCount++;
                        // Skip every token after this one
                        forward(&t, 1 - paramCount);
                        break;
                        // e.g.: $ goto
                    case C_GOTO:
                        // Fetch successor
                        fetch(&t);
                        // Parse argument (label)
                        // e.g.: $ goto 3b
                        if (parse_equation(&t)) paramCount++;
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
                if (isInsideLabel) report_error(E_VARDEC_INSIDE_LABEL);
                // Fetch successor
                fetch(&t);
                // Check if it has a variable name
                // e.g.: $ tryte foo
                if (t.tag == T_NAME) {
                    // Push name
                    push(t);
                    // Fetch successor
                    fetch(&t);
                } else {
                    // Throw error
                    report_error(E_EXPECTED_NAME_VARDEC);
                    // Try to fix it by pushing a dummy variable name
                    TOKEN tDummy = NEW_TOKEN;
                    tDummy.tag = T_NAME;
                    push(tDummy);
                }

                // Check if it has an assertion
                // e.g.: $ tryte foo =
                if (t.tag == T_ASSERTION) {
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
                // Check if it's outside a label
                if (!isInsideLabel) report_error(E_ASSERTION_OUTSIDE_LABEL);

                // Push assignable entity
                push(t);
                // Fetch successor
                fetch(&t);

                // Check if it has an assertion
                // e.g.: $ 1a =
                // e.g.: $ foo =
                if (t.tag == T_ASSERTION) {
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
                // Check if it's a reserved label
                if (*((uint8_t *)t.content) != L_CUSTOM)
                    report_error(E_INVALID_RESERVED_LABEL);
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
                while (t.tag == T_NEWLINE);
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

    if (noCompile) return 0;

    for (uint64_t i = 0; i < height; i++) {
        putt(stack[i].tag);
        if (stack[i].tag != T_NEWLINE) puts(" ");
    }
    puts("\n");
}