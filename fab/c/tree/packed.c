#include <stdlib.h>

#include "tree/packed.h"
#include "tree/tree.h"

#include "tree/node/node.h"

PackedTree* make_packed(MathTree* tree)
{
    if (!tree)  return NULL;

    int num_levels = tree->num_levels;

    PackedTree* packed = malloc(sizeof(PackedTree));

    (*packed) = (PackedTree) {
        .nodes      = num_levels ?
                        calloc(num_levels, sizeof(Node**)) : NULL,
        .active     = num_levels ?
                        calloc(num_levels, sizeof(unsigned)) : NULL,
        .disabled   = num_levels ?
                        calloc(num_levels, sizeof(ustack*)) : NULL,
        .num_levels = num_levels,
        .head       = tree->head
    };

    // Copy all nodes into the packed tree.
    for (unsigned level=0; level < tree->num_levels; ++level) {
        for (unsigned op=0; op < LAST_OP; ++op) {
            packed->active[level] += tree->active[level][op];
        }
        packed->nodes[level] = malloc(sizeof(Node*)*packed->active[level]);
        int index = 0;
        for (unsigned op=0; op < LAST_OP; ++op) {
            for (unsigned n=0; n < tree->active[level][op]; ++n) {
                packed->nodes[level][index++] = tree->nodes[level][op][n];
            }
        }
    }
    return packed;
}

void free_packed(PackedTree* packed)
{
    if (packed == NULL) return;

    for (unsigned level=0; level < packed->num_levels; ++level) {
        free(packed->nodes[level]);
    }

    free(packed->nodes);
    free(packed->active);
    free(packed->disabled);

    free(packed);
}

////////////////////////////////////////////////////////////////////////////////

_STATIC_
void disable_node(PackedTree* tree, int level, int n)
{
    Node* node = tree->nodes[level][n];

    // Fill all of the result slots with this value
    fill_results(node, node->results.i.upper);

    // Figure out where we should swap this node to.
    int back = --tree->active[level];
    Node* swap = tree->nodes[level][back];

    // And execute the swap.
    tree->nodes[level][n--]  = swap;
    tree->nodes[level][back] = node;

    // Finally, increase the count of disabled nodes
    tree->disabled[level]->count++;
}


void disable_nodes_binary(PackedTree* tree)
{
    for (int level=0; level < tree->num_levels; ++level) {
        for (int n=0; n < tree->active[level]; ++n) {
            tree->nodes[level][n]->flags |= NODE_BOOLEAN;
        }
    }

    for (int level=tree->num_levels-1; level >= 0; --level) {

        for (int n=0; n < tree->active[level]; ++n) {
            Node* node = tree->nodes[level][n];

            if ((node->flags & NODE_BOOLEAN) &&
                (node->results.i.lower >= 0 || node->results.i.upper < 0))
            {
                disable_node(tree, level, n--);
                node->flags = 0;
            }

            // If a node isn't binary, or it has a non-binary opcode,
            // then mark that children aren't binary.
            if (!(node->flags & NODE_BOOLEAN) ||
                    (node->opcode != OP_MIN &&
                     node->opcode != OP_MAX &&
                     node->opcode != OP_NEG))
            {
                if (node->lhs)  node->lhs->flags &= ~NODE_BOOLEAN;
                if (node->rhs)  node->rhs->flags &= ~NODE_BOOLEAN;
            }
        }
    }
}



void disable_nodes(PackedTree* tree)
{
    // Mark every node as ignored and binary.
    // We'll then go down the tree and mark nodes as uncacheable.
    for (int level=0; level < tree->num_levels; ++level) {
        for (int n=0; n < tree->active[level]; ++n) {
            tree->nodes[level][n]->flags |= NODE_IGNORED;
        }
    }
    tree->head->flags &= ~NODE_IGNORED;

    for (int level=tree->num_levels-1; level >= 0; --level) {

        // Save the number of nodes disabled in this pass so that
        // we can reverse the operation later.
        ustack* tmp = tree->disabled[level];
        tree->disabled[level] = malloc(sizeof(ustack));
        *(tree->disabled[level]) = (ustack){0, tmp};

        for (int n=0; n < tree->active[level]; ++n) {
            Node* node = tree->nodes[level][n];

            // If this node is marked, swap it to the back of the list
            // and decrement the active nodes count.
            if (node->flags & NODE_IGNORED) {
                disable_node(tree, level, n--);
                node->flags = 0;
            }

            // If this is a max or min node, then we might need to
            // only keep one branch active (if that branch is definitely
            // larger/smaller than the other branch)
            else if (node->opcode == OP_MAX) {
                if (node->lhs->results.i.lower >=
                        node->rhs->results.i.upper) {
                    node->lhs->flags &= ~NODE_IGNORED;
                } else if (node->rhs->results.i.lower >=
                        node->lhs->results.i.upper) {
                    node->rhs->flags &= ~NODE_IGNORED;
                } else {
                    node->lhs->flags &= ~NODE_IGNORED;
                    node->rhs->flags &= ~NODE_IGNORED;
                }
            } else if (node->opcode == OP_MIN) {
                if (node->lhs->results.i.upper <=
                        node->rhs->results.i.lower) {
                    node->lhs->flags &= ~NODE_IGNORED;
                } else if (node->rhs->results.i.upper <=
                        node->lhs->results.i.lower) {
                    node->rhs->flags &= ~NODE_IGNORED;
                } else {
                    node->lhs->flags &= ~NODE_IGNORED;
                    node->rhs->flags &= ~NODE_IGNORED;
                }
            }

            // Other node types need to keep both branches active
            else {
                if (node->lhs)  node->lhs->flags &= ~NODE_IGNORED;
                if (node->rhs)  node->rhs->flags &= ~NODE_IGNORED;
            }
        }
    }
}


void enable_nodes(PackedTree* tree)
{
    for (int level=tree->num_levels-1; level >= 0; --level) {
        tree->active[level] += tree->disabled[level]->count;
        ustack* next = tree->disabled[level]->next;
        free(tree->disabled[level]);
        tree->disabled[level] = next;
    }
}

uint8_t active_axes(const PackedTree* const tree)
{
    if (!tree->num_levels)  return 0;

    uint8_t active = 0;
    if (tree->num_levels) {
        for (int a=0; a < tree->active[0]; ++a) {
            switch (tree->nodes[0][a]->opcode) {
                case OP_X:  active |= (1 << 2); break;
                case OP_Y:  active |= (1 << 1); break;
                case OP_Z:  active |= (1 << 0); break;
                default: ;
            }
        }
    }

    return active;
}
