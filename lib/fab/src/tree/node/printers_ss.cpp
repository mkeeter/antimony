#include <sstream>

#include "fab/tree/node/printers_ss.h"
#include "fab/tree/node/node.h"

static std::string add_pss(Node* n)
{
    std::stringstream ss;
    ss << "(" << print_node_ss(n->lhs) << "+"
              << print_node_ss(n->rhs) << ")";
    return ss.str();
}

static std::string sub_pss(Node* n)
{
    std::stringstream ss;
    ss << "(" << print_node_ss(n->lhs) << "-"
              << print_node_ss(n->rhs) << ")";
    return ss.str();
}

static std::string mul_pss(Node* n)
{
    std::stringstream ss;
    ss << "(" << print_node_ss(n->lhs) << "*"
              << print_node_ss(n->rhs) << ")";
    return ss.str();
}

static std::string div_pss(Node* n)
{
    std::stringstream ss;
    ss << "(" << print_node_ss(n->lhs) << "/"
              << print_node_ss(n->rhs) << ")";
    return ss.str();
}

static std::string min_pss(Node* n)
{
    std::stringstream ss;
    ss << "min(" << print_node_ss(n->lhs) << ", "
              << print_node_ss(n->rhs) << ")";
    return ss.str();
}

static std::string max_pss(Node* n)
{
    std::stringstream ss;
    ss << "max(" << print_node_ss(n->lhs) << ", "
              << print_node_ss(n->rhs) << ")";
    return ss.str();
}

static std::string pow_pss(Node* n)
{
    std::stringstream ss;
    ss << "pow(" << print_node_ss(n->lhs) << ", "
              << print_node_ss(n->rhs) << ")";
    return ss.str();
}

////////////////////////////////////////////////////////////////////////////////

static std::string square_pss(Node* n)
{
    std::stringstream ss;
    ss << "pow(" << print_node_ss(n->lhs) << ", 2)";
    return ss.str();
}

static std::string sqrt_pss(Node* n)
{
    std::stringstream ss;
    ss << "sqrt(" << print_node_ss(n->lhs) << ")";
    return ss.str();
}

static std::string sin_pss(Node* n)
{
    std::stringstream ss;
    ss << "sin(" << print_node_ss(n->lhs) << ")";
    return ss.str();
}

static std::string cos_pss(Node* n)
{
    std::stringstream ss;
    ss << "cos(" << print_node_ss(n->lhs) << ")";
    return ss.str();
}

static std::string tan_pss(Node* n)
{
    std::stringstream ss;
    ss << "tan(" << print_node_ss(n->lhs) << ")";
    return ss.str();
}

static std::string asin_pss(Node* n)
{
    std::stringstream ss;
    ss << "asin(" << print_node_ss(n->lhs) << ")";
    return ss.str();
}

static std::string acos_pss(Node* n)
{
    std::stringstream ss;
    ss << "acos(" << print_node_ss(n->lhs) << ")";
    return ss.str();
}

static std::string atan_pss(Node* n)
{
    std::stringstream ss;
    ss << "atan(" << print_node_ss(n->lhs) << ")";
    return ss.str();
}

static std::string neg_pss(Node* n)
{
    std::stringstream ss;
    ss << "-(" << print_node_ss(n->lhs) << ")";
    return ss.str();
}

static std::string exp_pss(Node* n)
{
    std::stringstream ss;
    ss << "exp(" << print_node_ss(n->lhs) << ")";
    return ss.str();
}

////////////////////////////////////////////////////////////////////////////////

static std::string constant_pss(Node* n)
{
    std::stringstream ss;
    ss << n->results.f;
    return ss.str();
}

static std::string X_pss(Node* n)
{
    (void)n;
    return "X";
}

static std::string Y_pss(Node* n)
{
    (void)n;
    return "Y";
}

static std::string Z_pss(Node* n)
{
    (void)n;
    return "Z";
}

////////////////////////////////////////////////////////////////////////////////

std::string print_node_ss(Node* n)
{
    switch (n->opcode) {
        case OP_ADD:    return add_pss(n);
        case OP_SUB:    return sub_pss(n);
        case OP_MUL:    return mul_pss(n);
        case OP_DIV:    return div_pss(n);
        case OP_MIN:    return min_pss(n);
        case OP_MAX:    return max_pss(n);
        case OP_POW:    return pow_pss(n);

        case OP_SQUARE: return square_pss(n);
        case OP_SQRT:   return sqrt_pss(n);
        case OP_SIN:    return sin_pss(n);
        case OP_COS:    return cos_pss(n);
        case OP_TAN:    return tan_pss(n);
        case OP_ASIN:   return asin_pss(n);
        case OP_ACOS:   return acos_pss(n);
        case OP_ATAN:   return atan_pss(n);
        case OP_NEG:    return neg_pss(n);
        case OP_EXP:    return exp_pss(n);

        case OP_CONST:  return constant_pss(n);
        case OP_X:      return X_pss(n);
        case OP_Y:      return Y_pss(n);
        case OP_Z:      return Z_pss(n);
        default:
            return "Unknown opcode!";
    }
}
