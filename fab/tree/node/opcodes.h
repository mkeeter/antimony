#ifndef OPCODES_H
#define OPCODES_H

#ifdef __cplusplus
extern "C" {
#endif

/** @enum Opcode_
    @brief Node operations
*/
typedef enum Opcode_ {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MIN,
    OP_MAX,
    OP_POW,

    OP_ABS,
    OP_SQUARE,
    OP_SQRT,
    OP_SIN,
    OP_COS,
    OP_TAN,
    OP_ASIN,
    OP_ACOS,
    OP_ATAN,
    OP_NEG,
    OP_EXP,

    OP_X,
    OP_Y,
    OP_Z,
    OP_CONST,

    LAST_OP
} Opcode;

/**@var OPCODE_NAMES
Names of node operations */
extern const char* OPCODE_NAMES[];

/** @returns Node label for a dot graph */
const char* dot_symbol(Opcode op);

/** @returns Node color for a dot graph */
const char* dot_color(Opcode op);

/** @returns Node font size for a dot graph */
int dot_fontsize(Opcode op);

#ifdef __cplusplus
}
#endif

#endif
