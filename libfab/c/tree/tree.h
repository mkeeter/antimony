#ifndef TREE_H
#define TREE_H

#include "tree/node/opcodes.h"

/** @struct MathTree_
    @brief A structure containing nodes organized by rank and opcode.
*/
typedef struct MathTree_ {
    /** @var nodes
    Array of nodes, indexed by [level][opcode][node] */
    struct Node_** ((*nodes)[LAST_OP]);

    /** @var active
    Active node count, indexed by [level][opcode] */
    unsigned       (*active)[LAST_OP];


    /** @var constants
    Array of constant nodes */
    struct Node_ **constants;

    /** @var num_constants
    Size of contants array */
    unsigned       num_constants;

    /** @var head
    Pointer to tree head*/
    struct Node_* head;

    /** @var num_levels
    Number of levels in the tree */
    unsigned num_levels;
} MathTree;


/** @brief Creates a new tree, allocating the appropriate arrays.
    @param num_levels Number of levels in tree
    @param num_constants Number of constants in tree
 */
MathTree* new_tree(unsigned num_levels, unsigned num_constants);

/** @brief Frees a tree and all of its nodes
    @param tree Target tree
*/
void free_tree(MathTree* tree);



/** @brief Prints a math tree to stdout. */
void print_tree(MathTree* tree);
/** @brief Verbosely prints a math tree to stdout. */
void print_tree_verbose(MathTree* tree);


/** @brief Prints a math tree to a given file descriptor. */
void fprint_tree(MathTree* tree, int fd);
/** @brief Verbosely prints a math tree to a given file descriptor. */
void fprint_tree_verbose(MathTree* tree, int fd);


/** @brief Saves a graphviz array representation of the tree to the given file
*/
void dot_arrys(const MathTree* const tree, const char* filename);


/** @brief Clones a tree and all of its nodes
    @details The tree must not have any deactivated nodes.
*/
MathTree* clone_tree(MathTree* orig);

#endif
