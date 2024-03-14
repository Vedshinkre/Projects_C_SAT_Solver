#include "tseitin.h"

#include <stdio.h>

#include "cnf.h"
#include "err.h"
#include "list.h"
#include "propformula.h"
#include "util.h"
#include "variables.h"

/**
 * Inserts a clause with one literal into the CNF.
 *
 * @param vt   the underlying variable table
 * @param cnf  a formula
 * @param a    a literal
 */
void addUnaryClause(VarTable* vt, CNF* cnf, Literal a) {
    Clause* clause = mkTernaryClause(vt, a, 0, 0);
    addClauseToCNF(cnf, clause);
}

/**
 * Inserts a clause with two literals into the CNF.
 *
 * @param vt   the underlying variable table
 * @param cnf  a formula
 * @param a    the first literal
 * @param b    the second literal
 */
void addBinaryClause(VarTable* vt, CNF* cnf, Literal a, Literal b) {
    Clause* clause = mkTernaryClause(vt, a, b, 0);
    addClauseToCNF(cnf, clause);
}

/**
 * Inserts a clause with three literals into the CNF.
 *
 * @param vt   the underlying variable table
 * @param cnf  a formula
 * @param a    the first literal
 * @param b    the second literal
 * @param c    the third literal
 */
void addTernaryClause(VarTable* vt, CNF* cnf, Literal a, Literal b, Literal c) {
    Clause* clause = mkTernaryClause(vt, a, b, c);
    addClauseToCNF(cnf, clause);
}

/**
 * Adds clauses for a propositional formula to a CNF.
 *
 * For a propositional formula pf, clauses that are added that are equivalent to
 *
 *     x <=> pf
 *
 * where x is usually a fresh variable. This variable is also returned.
 *
 * @param vt   the underlying variable table
 * @param cnf  a formula
 * @param pf   a propositional formula
 * @return     the variable x, as described above
 */
VarIndex addClauses(VarTable* vt, CNF* cnf, const PropFormula* pf) {
    switch (pf->kind) {
        case VAR: {
            return pf->data.var;
        }
        case AND: {
            VarIndex left = addClauses(vt, cnf, pf->data.operands[0]);
            VarIndex right = addClauses(vt, cnf, pf->data.operands[1]);
            VarIndex result = mkFreshVariable(vt);

            addBinaryClause(vt, cnf, -result, left);
            addBinaryClause(vt, cnf, -result, right);
            addTernaryClause(vt, cnf, -left, -right, result);

            return result;
        }
        case OR: {
            VarIndex left = addClauses(vt, cnf, pf->data.operands[0]);
            VarIndex right = addClauses(vt, cnf, pf->data.operands[1]);
            VarIndex result = mkFreshVariable(vt);

            addTernaryClause(vt, cnf, -result, left, right);
            addBinaryClause(vt, cnf, -left, result);
            addBinaryClause(vt, cnf, -right, result);

            return result;
        }
        case IMPLIES: {
            VarIndex left = addClauses(vt, cnf, pf->data.operands[0]);
            VarIndex right = addClauses(vt, cnf, pf->data.operands[1]);
            VarIndex result = mkFreshVariable(vt);

            addTernaryClause(vt, cnf, -result, -left, right);
            addBinaryClause(vt, cnf, left, result);
            addBinaryClause(vt, cnf, -right, result);

            return result;
        }
        case EQUIV: {
            VarIndex left = addClauses(vt, cnf, pf->data.operands[0]);
            VarIndex right = addClauses(vt, cnf, pf->data.operands[1]);
            VarIndex result = mkFreshVariable(vt);

            addTernaryClause(vt, cnf, -result, -left, right);
            addTernaryClause(vt, cnf, -result, -right, left);
            addTernaryClause(vt, cnf, result, -left, -right);
            addTernaryClause(vt, cnf, result, left, right);

            return result;
        }
        case NOT: {
            VarIndex left = addClauses(vt, cnf, pf->data.single_op);
            VarIndex result = mkFreshVariable(vt);

            addBinaryClause(vt, cnf, -result, -left);
            addBinaryClause(vt, cnf, left, result);

            return result;
        }
        default:
            break;
    }

    return 0;  // Return a default value if no case matches
}

CNF* getCNF(VarTable* vt, const PropFormula* f) {
    CNF* res = mkCNF();

    VarIndex x = addClauses(vt, res, f);

    addUnaryClause(vt, res, x);

    return res;
}
