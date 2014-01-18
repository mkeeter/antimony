#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tree/node/node.h"
#include "tree/node/printers.h"
#include "tree/math/math_f.h"

#include "util/switches.h"

// Non-recursively clone a node.
Node* clone_node(Node* n)
{
    // Allocate memory and copy everything over
    Node* clone = malloc(sizeof(Node));
    memcpy(clone, n, sizeof(Node));

    // Update children clone pointers
    if (n->lhs) clone->lhs = n->lhs->clone_address;
    if (n->rhs) clone->rhs = n->rhs->clone_address;

    // Record the address of the new clone, so that clones of
    // its parents can be adjusted to point in the right place
    n->clone_address = clone;

    return clone;
}

////////////////////////////////////////////////////////////////////////////////

Node* binary_n(Node* lhs, Node* rhs, float (*f)(float, float), Opcode op)
{
    Node* n = malloc(sizeof(Node));

    _Bool constant = (lhs->flags & NODE_CONSTANT) &&
                     (rhs->flags & NODE_CONSTANT);

    *n = (Node) {
        .opcode     = constant ? OP_CONST : op,
        .rank       = constant ? 0 : 1 + (lhs->rank > rhs->rank ?
                                          lhs->rank : rhs->rank),
        .flags      = constant ? NODE_CONSTANT : 0,
        .lhs        = constant ? NULL : lhs,
        .rhs        = constant ? NULL : rhs,
        .clone_address = NULL,
    };

    if (constant) {
        fill_results(n, (*f)(lhs->results.f, rhs->results.f));
    }

    return n;
}

Node* add_n(Node* lhs, Node* rhs) { return binary_n(lhs, rhs, add_f, OP_ADD); }
Node* sub_n(Node* lhs, Node* rhs) { return binary_n(lhs, rhs, sub_f, OP_SUB); }
Node* mul_n(Node* lhs, Node* rhs) { return binary_n(lhs, rhs, mul_f, OP_MUL); }
Node* div_n(Node* lhs, Node* rhs) { return binary_n(lhs, rhs, div_f, OP_DIV); }

Node* min_n(Node* lhs, Node* rhs) { return binary_n(lhs, rhs, min_f, OP_MIN); }
Node* max_n(Node* lhs, Node* rhs) { return binary_n(lhs, rhs, max_f, OP_MAX); }
Node* pow_n(Node* lhs, Node* rhs) { return binary_n(lhs, rhs, pow_f, OP_POW); }


////////////////////////////////////////////////////////////////////////////////

Node* unary_n(Node* arg, float (*f)(float), Opcode op)
{
    Node* n = malloc(sizeof(Node));

    _Bool constant = arg->flags & NODE_CONSTANT;

    *n = (Node) {
        .opcode     = constant ? OP_CONST : op,
        .rank       = constant ? 0 : 1 + arg->rank,
        .flags      = constant ? NODE_CONSTANT : 0,
        .lhs        = constant ? NULL : arg,
        .rhs        = NULL,
        .clone_address = NULL,
    };

    if (constant) {
        fill_results(n, (*f)(arg->results.f));
    }
    return n;
}

Node* abs_n(Node* child) { return unary_n(child, abs_f, OP_ABS); }
Node* square_n(Node* child) { return unary_n(child, square_f, OP_SQUARE); }
Node* sqrt_n(Node* child) { return unary_n(child, sqrt_f, OP_SQRT); }
Node* sin_n(Node* child) { return unary_n(child, sin_f, OP_SIN); }
Node* cos_n(Node* child) { return unary_n(child, cos_f, OP_COS); }
Node* tan_n(Node* child) { return unary_n(child, tan_f, OP_TAN); }
Node* asin_n(Node* child) { return unary_n(child, asin_f, OP_ASIN); }
Node* acos_n(Node* child) { return unary_n(child, acos_f, OP_ACOS); }
Node* atan_n(Node* child) { return unary_n(child, atan_f, OP_ATAN); }
Node* neg_n(Node* child) { return unary_n(child, neg_f, OP_NEG); }
Node* exp_n(Node* child) { return unary_n(child, abs_f, OP_EXP); }

////////////////////////////////////////////////////////////////////////////////

Node* nonary_n(Opcode op)
{
    Node* n = malloc(sizeof(Node));

    *n = (Node) {
        .opcode     = op,
        .rank       = 0,
        .flags      = 0,
        .lhs        = NULL,
        .rhs        = NULL,
        .clone_address = NULL,
    };

    return n;
}

Node* constant_n(float value)
{
    Node* n = nonary_n(OP_CONST);
    n->flags = NODE_CONSTANT;
    fill_results(n, value);
    return n;
}

Node* X_n()
{
    return nonary_n(OP_X);
}

Node* Y_n()
{
    return nonary_n(OP_Y);
}

Node* Z_n()
{
    return nonary_n(OP_Z);
}
