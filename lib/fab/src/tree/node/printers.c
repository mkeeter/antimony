#include "fab/tree/node/printers.h"
#include "fab/tree/node/node.h"

////////////////////////////////////////////////////////////////////////////////

static void base_p(Node* n, FILE* f)
{
    fprintf(f, ANSI_COLOR_GRAY "[rank=%i; flags=%u]" ANSI_COLOR_RESET, n->rank, n->flags);
}

static void add_p(Node* n, FILE* f)
{
    fprintf(f, "(");
    fprint_node(n->lhs, f);
    fprintf(f, "+");
    base_p(n, f);
    fprint_node(n->rhs, f);
    fprintf(f, ")");
}

static void sub_p(Node* n, FILE* f)
{
    fprintf(f, "(");
    fprint_node(n->lhs, f);
    fprintf(f, "-");
    base_p(n, f);
    fprint_node(n->rhs, f);
    fprintf(f, ")");
}

static void mul_p(Node* n, FILE* f)
{
    fprintf(f, "(");
    fprint_node(n->lhs, f);
    fprintf(f, "*");
    base_p(n, f);
    fprint_node(n->rhs, f);
    fprintf(f, ")");
}

static void div_p(Node* n, FILE* f)
{
    fprintf(f, "(");
    fprint_node(n->lhs, f);
    fprintf(f, "/");
    base_p(n, f);
    fprint_node(n->rhs, f);
    fprintf(f, ")");
}

static void min_p(Node* n, FILE* f)
{
    fprintf(f, "min");
    base_p(n, f);
    fprintf(f, "(");
    fprint_node(n->lhs, f);
    fprintf(f, ", ");
    fprint_node(n->rhs, f);
    fprintf(f, ")");
}

static void max_p(Node* n, FILE* f)
{
    fprintf(f, "max");
    base_p(n, f);
    fprintf(f, "(");
    fprint_node(n->lhs, f);
    fprintf(f, ", ");
    fprint_node(n->rhs, f);
    fprintf(f, ")");
}

static void pow_p(Node* n, FILE* f)
{
    fprintf(f, "pow");
    base_p(n, f);
    fprintf(f, "(");
    fprint_node(n->lhs, f);
    fprintf(f, ", ");
    fprint_node(n->rhs, f);
    fprintf(f, ")");
}

////////////////////////////////////////////////////////////////////////////////

static void square_p(Node* n, FILE* f)
{
    fprintf(f, "sqr");
    base_p(n, f);
    fprintf(f, "(");
    fprint_node(n->lhs, f);
    fprintf(f, ")");
}

static void sqrt_p(Node* n, FILE* f)
{
    fprintf(f, "sqrt");
    base_p(n, f);
    fprintf(f, "(");
    fprint_node(n->lhs, f);
    fprintf(f, ")");
}

static void sin_p(Node* n, FILE* f)
{
    fprintf(f, "sin");
    base_p(n, f);
    fprintf(f, "(");
    fprint_node(n->lhs, f);
    fprintf(f, ")");
}

static void cos_p(Node* n, FILE* f)
{
    fprintf(f, "cos");
    base_p(n, f);
    fprintf(f, "(");
    fprint_node(n->lhs, f);
    fprintf(f, ")");
}

static void tan_p(Node* n, FILE* f)
{
    fprintf(f, "tan");
    base_p(n, f);
    fprintf(f, "(");
    fprint_node(n->lhs, f);
    fprintf(f, ")");
}

static void asin_p(Node* n, FILE* f)
{
    fprintf(f, "asin");
    base_p(n, f);
    fprintf(f, "(");
    fprint_node(n->lhs, f);
    fprintf(f, ")");
}

static void acos_p(Node* n, FILE* f)
{
    fprintf(f, "acos");
    base_p(n, f);
    fprintf(f, "(");
    fprint_node(n->lhs, f);
    fprintf(f, ")");
}

static void atan_p(Node* n, FILE* f)
{
    fprintf(f, "atan");
    base_p(n, f);
    fprintf(f, "(");
    fprint_node(n->lhs, f);
    fprintf(f, ")");
}

static void neg_p(Node* n, FILE* f)
{
    fprintf(f, "-");
    base_p(n, f);
    fprint_node(n->lhs, f);
}

static void exp_p(Node* n, FILE* f)
{
    fprintf(f, "exp");
    base_p(n, f);
    fprintf(f, "(");
    fprint_node(n->lhs, f);
    fprintf(f, ")");
}

////////////////////////////////////////////////////////////////////////////////

static void constant_p(Node* n, FILE* f)
{
    fprintf(f, "%.3g", n->results.f);
}

static void X_p(Node* n, FILE* f)
{
    fprintf(f, "X");
}

static void Y_p(Node* n, FILE* f)
{
    fprintf(f, "Y");
}

static void Z_p(Node* n, FILE* f)
{
    fprintf(f, "Z");
}

////////////////////////////////////////////////////////////////////////////////

void print_node(Node* n)
{
    fprint_node(n, stdout);
}

void fdprint_node(Node* n, int fd)
{
    FILE* f = fdopen(fd, "w");
    fprint_node(n, f);
    fclose(f);
}

void fprint_node(Node* n, FILE* f)
{
    switch (n->opcode) {
        case OP_ADD:    add_p(n, f); break;
        case OP_SUB:    sub_p(n, f); break;
        case OP_MUL:    mul_p(n, f); break;
        case OP_DIV:    div_p(n, f); break;
        case OP_MIN:    min_p(n, f); break;
        case OP_MAX:    max_p(n, f); break;
        case OP_POW:    pow_p(n, f); break;

        case OP_SQUARE: square_p(n, f); break;
        case OP_SQRT:   sqrt_p(n, f); break;
        case OP_SIN:    sin_p(n, f); break;
        case OP_COS:    cos_p(n, f); break;
        case OP_TAN:    tan_p(n, f); break;
        case OP_ASIN:   asin_p(n, f); break;
        case OP_ACOS:   acos_p(n, f); break;
        case OP_ATAN:   atan_p(n, f); break;
        case OP_NEG:    neg_p(n, f); break;
        case OP_EXP:    exp_p(n, f); break;

        case OP_CONST:  constant_p(n, f); break;
        case OP_X:      X_p(n, f); break;
        case OP_Y:      Y_p(n, f); break;
        case OP_Z:      Z_p(n, f); break;
        default:
            fprintf(f, "Unknown opcode!: %i\n", n->opcode);
    }
}
