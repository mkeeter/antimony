#ifndef NODE_H
#define NODE_H

#include <stdbool.h>

#include "fab/tree/node/results.h"
#include "fab/tree/node/opcodes.h"
#include "fab/util/interval.h"
#include "fab/util/region.h"

#define NODE_CONSTANT   1
#define NODE_IGNORED    2
#define NODE_BOOLEAN    4
#define NODE_IN_TREE    8

#ifdef __cplusplus
extern "C" {
#endif

/** @struct Node_
    @brief Recursive data structure defining a node in a math tree.
*/
typedef struct Node_ {
    /** @var opcode
    Node operation */
    Opcode opcode;

    /** @var results
    Saved results from most recent evaluation */
    Results  results;

    /** @var rank
    Rank of the node in the tree. */
    int rank;

    /** @var flags
    Flags (combination of be NODE_CONSTANT, NODE_IGNORED,
    NODE_BOOLEAN, and NODE_IN_TREE) */
    uint8_t flags;

    /** @var lhs
    Left-hand child node (or NULL)
    */
    struct Node_* lhs;

    /** @var rhs
    Right-hand child node (or NULL)
    */
    struct Node_* rhs;

    /** @var clone_address
    Most recent place to which this node was cloned
    */
    struct Node_* clone_address;
} Node;


/** @brief  Clones a single node (non-recursively).
    @details Looks up clone_address of children, which must
    be populated with a sane value.
    @param n Node to clone
    @returns Pointer to clone
*/
Node* clone_node(Node* n);

////////////////////////////////////////////////////////////////////////////////
// Node constructors
////////////////////////////////////////////////////////////////////////////////

// Binary operations
Node* add_n(Node* left, Node* right);
Node* sub_n(Node* left, Node* right);
Node* mul_n(Node* left, Node* right);
Node* div_n(Node* left, Node* right);

Node* min_n(Node* left, Node* right);
Node* max_n(Node* left, Node* right);
Node* pow_n(Node* left, Node* right);

// Unary arithmetic operators
Node* abs_n(Node* n);
Node* square_n(Node* n);
Node* sqrt_n(Node* n);
Node* sin_n(Node* n);
Node* cos_n(Node* n);
Node* tan_n(Node* n);
Node* asin_n(Node* n);
Node* acos_n(Node* n);
Node* atan_n(Node* n);
Node* neg_n(Node* n);
Node* exp_n(Node* n);

// Constants
Node* constant_n(float value);

// Variables
Node* X_n(void);
Node* Y_n(void);
Node* Z_n(void);

#ifdef __cplusplus
}
#endif

#endif
