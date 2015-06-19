#include <stdio.h>

#include "fab/tree/tree.h"
#include "fab/tree/eval.h"

#include "fab/tree/node/node.h"

#include "fab/tree/math/math_f.h"
#include "fab/tree/math/math_i.h"
#include "fab/tree/math/math_r.h"
#include "fab/tree/math/math_g.h"

float eval_f(MathTree* tree, const float x, const float y, const float z)
{
    Node* node = NULL;

    for (unsigned level=0; level < tree->num_levels; ++level) {
        for (unsigned n=0; n < tree->active[level]; ++n) {

            node = tree->nodes[level][n];

            float A = node->lhs ? node->lhs->results.f : 0,
                  B = node->rhs ? node->rhs->results.f : 0;

            switch (node->opcode) {
                case OP_ADD:    node->results.f = add_f(A, B); break;
                case OP_SUB:    node->results.f = sub_f(A, B); break;
                case OP_MUL:    node->results.f = mul_f(A, B); break;
                case OP_DIV:    node->results.f = div_f(A, B); break;
                case OP_MIN:    node->results.f = min_f(A, B); break;
                case OP_MAX:    node->results.f = max_f(A, B); break;
                case OP_POW:    node->results.f = pow_f(A, B); break;

                case OP_ABS:    node->results.f = abs_f(A); break;
                case OP_SQUARE: node->results.f = square_f(A); break;
                case OP_SQRT:   node->results.f = sqrt_f(A); break;
                case OP_SIN:    node->results.f = sin_f(A); break;
                case OP_COS:    node->results.f = cos_f(A); break;
                case OP_TAN:    node->results.f = tan_f(A); break;
                case OP_ASIN:   node->results.f = asin_f(A); break;
                case OP_ACOS:   node->results.f = acos_f(A); break;
                case OP_ATAN:   node->results.f = atan_f(A); break;
                case OP_NEG:    node->results.f = neg_f(A); break;
                case OP_EXP:    node->results.f = exp_f(A); break;

                case OP_X:      node->results.f = X_f(x); break;
                case OP_Y:      node->results.f = Y_f(y); break;
                case OP_Z:      node->results.f = Z_f(z); break;

                case OP_CONST:  break;
                default:
                    printf("Unknown opcode!\n");
            }
        }
    }
    return tree->head->results.f;
}

////////////////////////////////////////////////////////////////////////////////

Interval eval_i(MathTree* tree, const Interval X,
                                const Interval Y,
                                const Interval Z)
{
    Node* node = NULL;

    for (unsigned level=0; level < tree->num_levels; ++level) {
        for (unsigned n=0; n < tree->active[level]; ++n) {

            node = tree->nodes[level][n];

            Interval A = node->lhs ? node->lhs->results.i
                                   : (Interval) {.upper=0, .lower=0},
                     B = node->rhs ? node->rhs->results.i
                                   : (Interval) {.upper=0, .lower=0};

            switch (node->opcode) {
                case OP_ADD:    node->results.i = add_i(A, B); break;
                case OP_SUB:    node->results.i = sub_i(A, B); break;
                case OP_MUL:    node->results.i = mul_i(A, B); break;
                case OP_DIV:    node->results.i = div_i(A, B); break;
                case OP_MIN:    node->results.i = min_i(A, B); break;
                case OP_MAX:    node->results.i = max_i(A, B); break;
                case OP_POW:    node->results.i = pow_i(A, B); break;

                case OP_ABS:    node->results.i = abs_i(A); break;
                case OP_SQUARE: node->results.i = square_i(A); break;
                case OP_SQRT:   node->results.i = sqrt_i(A); break;
                case OP_SIN:    node->results.i = sin_i(A); break;
                case OP_COS:    node->results.i = cos_i(A); break;
                case OP_TAN:    node->results.i = tan_i(A); break;
                case OP_ASIN:   node->results.i = asin_i(A); break;
                case OP_ACOS:   node->results.i = acos_i(A); break;
                case OP_ATAN:   node->results.i = atan_i(A); break;
                case OP_NEG:    node->results.i = neg_i(A); break;
                case OP_EXP:    node->results.i = exp_i(A); break;

                case OP_CONST:  break;
                case OP_X:      node->results.i = X_i(X); break;
                case OP_Y:      node->results.i = Y_i(Y); break;
                case OP_Z:      node->results.i = Z_i(Z); break;
                default:
                    printf("Unknown opcode!\n");
            }
        }
    }

    return tree->head->results.i;
}

////////////////////////////////////////////////////////////////////////////////

float* eval_r(MathTree* tree, const Region r)
{
    Node* node = NULL;
    int c = r.voxels;

    for (unsigned level=0; level < tree->num_levels; ++level) {
        for (unsigned n=0; n < tree->active[level]; ++n) {

            node = tree->nodes[level][n];

            float *A = node->lhs ? node->lhs->results.r  : NULL,
                  *B = node->rhs ? node->rhs->results.r : NULL,
                  *R = node->results.r;

            switch (node->opcode) {
                case OP_ADD:    add_r(A, B, R, c); break;
                case OP_SUB:    sub_r(A, B, R, c); break;
                case OP_MUL:    mul_r(A, B, R, c); break;
                case OP_DIV:    div_r(A, B, R, c); break;
                case OP_MIN:    min_r(A, B, R, c); break;
                case OP_MAX:    max_r(A, B, R, c); break;
                case OP_POW:    pow_r(A, B, R, c); break;

                case OP_ABS:    abs_r(A, R, c); break;
                case OP_SQUARE: square_r(A, R, c); break;
                case OP_SQRT:   sqrt_r(A, R, c); break;
                case OP_SIN:    sin_r(A, R, c); break;
                case OP_COS:    cos_r(A, R, c); break;
                case OP_TAN:    tan_r(A, R, c); break;
                case OP_ASIN:   asin_r(A, R, c); break;
                case OP_ACOS:   acos_r(A, R, c); break;
                case OP_ATAN:   atan_r(A, R, c); break;
                case OP_NEG:    neg_r(A, R, c); break;
                case OP_EXP:    exp_r(A, R, c); break;

                case OP_CONST:  break;
                case OP_X:      X_r(r.X, R, c); break;
                case OP_Y:      Y_r(r.Y, R, c); break;
                case OP_Z:      Z_r(r.Z, R, c); break;
                default:
                    printf("Unknown opcode!\n");
            }
        }
    }

    return tree->head->results.r;
}

////////////////////////////////////////////////////////////////////////////////

derivative* eval_g(MathTree* tree, const Region r)
{
    Node* node = NULL;
    int c = r.voxels;

    for (unsigned level=0; level < tree->num_levels; ++level) {
        for (unsigned n=0; n < tree->active[level]; ++n) {

            node = tree->nodes[level][n];

            derivative *A = (derivative*)(node->lhs ? node->lhs->results.r
                                                    : NULL),
                       *B = (derivative*)(node->rhs ? node->rhs->results.r
                                                   : NULL),
                       *R = (derivative*)(node->results.r);

            switch (node->opcode) {
                case OP_ADD:    add_g(A, B, R, c); break;
                case OP_SUB:    sub_g(A, B, R, c); break;
                case OP_MUL:    mul_g(A, B, R, c); break;
                case OP_DIV:    div_g(A, B, R, c); break;
                case OP_MIN:    min_g(A, B, R, c); break;
                case OP_MAX:    max_g(A, B, R, c); break;
                case OP_POW:    pow_g(A, B, R, c); break;

                case OP_ABS:    abs_g(A, R, c); break;
                case OP_SQUARE: square_g(A, R, c); break;
                case OP_SQRT:   sqrt_g(A, R, c); break;
                case OP_SIN:    sin_g(A, R, c); break;
                case OP_COS:    cos_g(A, R, c); break;
                case OP_TAN:    tan_g(A, R, c); break;
                case OP_ASIN:   asin_g(A, R, c); break;
                case OP_ACOS:   acos_g(A, R, c); break;
                case OP_ATAN:   atan_g(A, R, c); break;
                case OP_NEG:    neg_g(A, R, c); break;
                case OP_EXP:    exp_g(A, R, c); break;

                case OP_CONST:  break;
                case OP_X:      X_g(r.X, R, c); break;
                case OP_Y:      Y_g(r.Y, R, c); break;
                case OP_Z:      Z_g(r.Z, R, c); break;
                default:
                    printf("Unknown opcode!\n");
            }
        }
    }

    return (derivative*)(tree->head->results.r);
}
