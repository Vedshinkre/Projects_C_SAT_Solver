#include "dpll.h"

// #include <cstddef>

#include "cnf.h"
#include "err.h"
#include "list.h"
#include "util.h"
#include "variables.h"

typedef enum Reason { CHOSEN, IMPLIED } Reason;

/**
 * Struct to represent an entry in the assignment stack. Should only be created
 * and freed by pushAssignment and popAssignment.
 */
typedef struct Assignment {
    VarIndex var;
    Reason reason;
} Assignment;

/**
 * Adds a new assignment to the assignment stack.
 *
 * @param stack  an assignment stack
 * @param var    the variable to assign
 * @param r      the reason for the assignment
 */
void pushAssignment(List* stack, VarIndex var, Reason r) {
    Assignment* a = (Assignment*)malloc(sizeof(Assignment));
    a->var = var;
    a->reason = r;
    push(stack, a);
}

/**
 * Removes the head element of an assignment stack and frees it.
 *
 * @param stack  an assignment stack
 */
void popAssignment(List* stack) {
    Assignment* a = (Assignment*)peek(stack);
    free(a);
    pop(stack);
}

/**
 * Führt eine Iteration des DPLL Algorithmus aus.
 *
 * @param vt       die zugrunde liegende Variablentabelle
 * @param stack    der Zuweisungsstack
 * @param cnf      die zu prüfende Formel
 * @return         1 falls der Algorithmus mit SAT terminieren sollte,
 *                 0 falls der Algorithmus weiterlaufen sollte,
 *                 -1 falls der Algorithmus mit UNSAT terminieren sollte
 */
/**
 * Performs one iteration of the DPLL algorithm.
 *
 * @param vt       the underlying variable table
 * @param stack    an assignment stack
 * @param cnf      the formula to check
 * @return         1 if the algorithm should terminate with SAT,
 *                 0 if the algorithm should continue,
 *                -1 if the algorithm should terminate with UNSAT
 */
int iterate(VarTable* vt, List* stack, CNF* cnf) {
    while (1) {
        if (cnf == NULL) {
            return 0;
        }

        if (evalCNF(cnf) == TRUE) {
            return 1;  // Abort - Formula is satisfiable
        }

        if (evalCNF(cnf) == FALSE) {
            while (!isEmpty(stack)) {
                Assignment* a = (Assignment*)peek(stack);
                if (a->reason == CHOSEN) {
                    updateVariableValue(vt, a->var, FALSE);
                    a->reason = IMPLIED;
                    return 0;
                } else if (a->reason == IMPLIED) {
                    updateVariableValue(vt, a->var, UNDEFINED);
                }
                popAssignment(stack);
            }

            return -1;
        }

        if (isEmpty(&cnf->clauses)) {
            return -1;  // Abort - Formula is unsatisfiable
        }

        ListIterator clauseIt = mkIterator(&cnf->clauses);
        while (isValid(&clauseIt)) {
            Clause* clause = (Clause*)getCurr(&clauseIt);

            Literal unitLiteral = getUnitLiteral(vt, clause);
            if (unitLiteral != 0) {
                if (unitLiteral > 0) {
                    updateVariableValue(vt, unitLiteral, TRUE);
                    pushAssignment(stack, unitLiteral, IMPLIED);
                    return 0;
                } else {
                    updateVariableValue(vt, -unitLiteral, FALSE);
                    pushAssignment(stack, -unitLiteral, IMPLIED);
                    return 0;
                }
                break;  // Exit the loop after handling one unit literal
            } else {
                next(&clauseIt);
            }
        }

        VarIndex variableIndex = getNextUndefinedVariable(vt);

        if (variableIndex != 0) {
            pushAssignment(stack, variableIndex, CHOSEN);
            updateVariableValue(vt, variableIndex, TRUE);
            return 0;
        } else {
            return 1;
        }
    }
}
char isSatisfiable(VarTable* vt, CNF* cnf) {
    List stack = mkList();

    int res;
    do {
        res = iterate(vt, &stack, cnf);
    } while (res == 0);

    while (!isEmpty(&stack)) {
        popAssignment(&stack);
    }

    return (res < 0) ? 0 : 1;
}
