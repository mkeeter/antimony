#ifndef PACKED_H
#define PACKED_H

#include <stdint.h>

#include "tree/tree.h"

/** @struct ustack_
    @brief A simple FIFO stack of unsigned integers.
*/
typedef struct ustack_ {
    /** @var count
    Stored value */
    unsigned count;

    /** @var next
    Next item in the stack */
    struct ustack_* next;
} ustack;

/** @struct PackedTree_
    @brief A structure containing nodes organized by rank
*/
typedef struct PackedTree_ {
    /** @var nodes
    Node pointers, stored in rows indexed by level */
    struct Node_*** nodes;

    /** @var active
    Number of active nodes, indexed by level */
    unsigned* active;

    /** @var disabled
    Stacks of disabled node counts, indexed by level */
    ustack**  disabled;

    /** @var num_levels
    Number of levels in this tree */
    unsigned num_levels;

    /** @var head
    Root of this tree */
    struct Node_* head;
} PackedTree;


/** @brief Converts a MathTree into a PackedTree
    @param tree A well-formed, deduplicated MathTree.
    @returns A PackedTree with the same nodes.
    @details Nodes are not copied; they point back to the original MathTree
*/
PackedTree* make_packed(struct MathTree_* tree);


/** @brief Frees a packed tree.
    @details Does not free nodes, since they should be
    pointers to the same nodes as the original MathTree.
*/
void free_packed(PackedTree* packed);


/** @brief Travels down the tree, disabling nodes whose values will not matter
    upon further spatial subdivision

    @details
    Nodes are disabled by swapping them to the back of their respective list
    and decrementing the active count (so that the evaluation loop doesn't
    touch them at all).

    A count of nodes disabled in this pass is stored on the top of the
    disabled stack (used by enable_nodes).
 */
void disable_nodes(PackedTree* tree);


/** @brief Disables nodes that won't affect the output truth value
    (i.e. whether it is larger or smaller than zero)

    @details
    Must be called after disable_nodes, otherwise the stacks won't be
    in place to store the number of disabled nodes.
*/
void disable_nodes_binary(PackedTree* tree);


/** @brief Enables nodes that were disabled on the most recent call to disable_nodes.

    @details
    Nodes are enabled by increasing the value of active[level][op], so that
    an evaluation continues further down the list.
*/
void enable_nodes(PackedTree* tree);

/** @brief Returns a bit mask containing active axes in a tree.
    @details
    The bit mask is of the form (x_active << 2) | (y_active << 1) | (z_active)
*/
uint8_t active_axes(const PackedTree* const tree);

#endif
