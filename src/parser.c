#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "err.h"
#include "lexer.h"
#include "list.h"
#include "propformula.h"
#include "util.h"
#include "variables.h"

/**
 * Assigns symbols to strings.
 *
 * Aborts the program with an error message if an invalid input is detected.
 *
 * @param str  a string to translate
 * @return     the resulting symbol
 */
FormulaKind toKind(const char* str) {
    if (strcmp(str, "||") == 0) {
        return OR;
    } else if (strcmp(str, "&&") == 0) {
        return AND;
    } else if (strcmp(str, "<=>") == 0) {
        return EQUIV;
    } else if (strcmp(str, "=>") == 0) {
        return IMPLIES;
    } else if (strcmp(str, "!") == 0) {
        return NOT;
    } else {
        return VAR;
    }
}

PropFormula* parseFormula(FILE* input, VarTable* vt) {
    List lamb = mkList();
    char* c;
    int i;
    while ((c = nextToken(input)) != NULL) {
        FormulaKind b = toKind(c);
        i = 0;  // Reset the value of 'i' for each new token

        if (b == VAR) {
            while (c[i] != '\0') {
                if ((c[i] > 47 && c[i] < 58) || (c[i] > 64 && c[i] < 91) ||
                    (c[i] > 96 && c[i] < 123)) {
                    // Valid character, do nothing
                } else {
                    err("Unacceptable character");
                    break;
                }
                i++;
            }
            PropFormula* co = mkVarFormula(vt, c);
            push(&lamb, co);
        } else if (b == AND || b == OR || b == EQUIV || b == IMPLIES) {
            int numElements = 0;
            ListIterator it = mkIterator(&lamb);

            while (isValid(&it)) {
                numElements++;
                next(&it);
            }

            if (numElements < 2) {
                err("Insufficient operands in the stack");
                break;  // Exit the loop to prevent further processing
            }

            PropFormula* rightOp = peek(&lamb);
            pop(&lamb);
            PropFormula* leftOp = peek(&lamb);
            pop(&lamb);
            PropFormula* formula = mkBinaryFormula(b, leftOp, rightOp);
            push(&lamb, formula);
            free(c);
        }

        else if (b == NOT) {
            int numElements = 0;
            ListIterator it = mkIterator(&lamb);

            while (isValid(&it)) {
                numElements++;
                next(&it);
            }

            if (numElements < 1) {
                err("Insufficient operands in the stack");
                break;  // Exit the loop to prevent further processing
            }

            PropFormula* operand = peek(&lamb);
            pop(&lamb);
            PropFormula* formula = mkUnaryFormula(b, operand);
            push(&lamb, formula);
            free(c);
        }

        // No need to free 'c' here since it is pushed onto the list
    }

    int numElements = 0;
    ListIterator it = mkIterator(&lamb);

    while (isValid(&it)) {
        numElements++;
        next(&it);
    }

    if (numElements != 1) {
        err("Invalid formula: Extra operands or insufficient operands in the "
            "stack");
    }

    PropFormula* result = peek(&lamb);
    clearList(&lamb);

    return result;
}