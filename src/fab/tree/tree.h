#ifndef TREE_H
#define TREE_H

#include <stdint.h>

#include "tree/node/opcodes.h"
#include "util/ustack.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @struct MathTree_
    @brief A structure containing nodes organized by rank and opcode.
*/
typedef struct MathTree_ {
    /** @var nodes
    Node pointers, stored in rows indexed by level */
    struct Node_*** nodes;

    /** @var active
    Number of active nodes, indexed by level */
    unsigned* active;

    /** @var disabled
    Stacks of disabled node counts, indexed by level */
    ustack*  disabled;

    /** @var num_levels
    Number of levels in this tree */
    unsigned num_levels;

    /** @var constants
    Array of constant nodes */
    struct Node_ **constants;

    /** @var num_constants
    Size of contants array */
    unsigned       num_constants;

    /** @var head
    Root of this tree */
    struct Node_* head;
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


/** @brief Prints a math tree to a given file descriptor. */
void fprint_tree(MathTree* tree, int fd);


/** @brief Clones a tree and all of its nodes.
    @details The tree must not have any deactivated nodes.
*/
MathTree* clone_tree(MathTree* orig);


/** @brief Travels down the tree, disabling nodes whose values will not matter
    upon further spatial subdivision

    @details
    Nodes are disabled by swapping them to the back of their respective list
    and decrementing the active count (so that the evaluation loop doesn't
    touch them at all).

    A count of nodes disabled in this pass is stored on the top of the
    disabled stack (used by enable_nodes).
 */
void disable_nodes(MathTree* tree);


/** @brief Disables nodes that won't affect the output truth value
    (i.e. whether it is larger or smaller than zero)

    @details
    Must be called after disable_nodes, otherwise the stacks won't be
    in place to store the number of disabled nodes.
*/
void disable_nodes_binary(MathTree* tree);


/** @brief Enables nodes that were disabled on the most recent call to disable_nodes.

    @details
    Nodes are enabled by increasing the value of active[level][op], so that
    an evaluation continues further down the list.
*/
void enable_nodes(MathTree* tree);

/** @brief Returns a bit mask containing active axes in a tree.
    @details
    The bit mask is of the form (x_active << 2) | (y_active << 1) | (z_active)
*/
uint8_t active_axes(const MathTree* const tree);

#ifdef __cplusplus
}
#endif

#endif
