#include "fab/tree/node/opcodes.h"

const char* OPCODE_NAMES[] = {
    "OP_ADD",
    "OP_SUB",
    "OP_MUL",
    "OP_DIV",
    "OP_MIN",
    "OP_MAX",
    "OP_POW",

    "OP_ABS",
    "OP_SQUARE",
    "OP_SQRT",
    "OP_SIN",
    "OP_COS",
    "OP_TAN",
    "OP_ASIN",
    "OP_ACOS",
    "OP_ATAN",
    "OP_NEG",
    "OP_EXP",

    "OP_X",
    "OP_Y",
    "OP_Z",
    "OP_CONST",

    "LAST_OP"
};

const char* dot_symbol(Opcode op) {
    switch (op) {
        case OP_ADD:    return "+";
        case OP_SUB:    return "−";
        case OP_MUL:    return "×";
        case OP_DIV:    return "/";
        case OP_MIN:    return "min";
        case OP_MAX:    return "max";
        case OP_POW:    return "pow";
        case OP_ABS:    return "abs";
        case OP_SQUARE: return "square";
        case OP_SQRT:   return "sqrt";
        case OP_SIN:    return "sin";
        case OP_COS:    return "cos";
        case OP_TAN:    return "tan";
        case OP_ASIN:   return "asin";
        case OP_ACOS:   return "acos";
        case OP_ATAN:   return "atan";
        case OP_NEG:    return "−";
        case OP_EXP:    return "exp";
        case OP_X:      return "X";
        case OP_Y:      return "Y";
        case OP_Z:      return "Z";
        case OP_CONST:  return "C";
        default:        return "";
    }
}

const char* dot_color(Opcode op) {
    switch (op) {
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_POW:
        case OP_ABS:
        case OP_SQUARE:
        case OP_SQRT:
        case OP_SIN:
        case OP_COS:
        case OP_TAN:
        case OP_ASIN:
        case OP_ACOS:
        case OP_ATAN:
        case OP_EXP:
            return "goldenrod";
        case OP_MIN:
        case OP_MAX:
        case OP_NEG:
            return "dodgerblue";
        case OP_X:
        case OP_Y:
        case OP_Z:
            return "red";
        case OP_CONST:
            return "green";
        default:
            return "black";
    }
}

int dot_fontsize(Opcode op) {
    switch (op) {
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_NEG:
        case OP_X:
        case OP_Y:
        case OP_Z:
        case OP_CONST:
            return 24;
        case OP_MIN:
        case OP_MAX:
        case OP_POW:
        case OP_ABS:
        case OP_SQUARE:
        case OP_SQRT:
        case OP_SIN:
        case OP_COS:
        case OP_TAN:
        case OP_ASIN:
        case OP_ACOS:
        case OP_ATAN:
        case OP_EXP:
            return 14;
        default:
            return 0;
    }
}
