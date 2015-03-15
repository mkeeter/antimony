#include <stdlib.h>

#include "tree/tree.h"

#include "tree/node/node.h"
#include "tree/node/printers.h"
#include "util/switches.h"

MathTree* new_tree(unsigned num_levels, unsigned num_constants)
{
    MathTree* tree = malloc(sizeof(MathTree));

    *tree = (MathTree){
        .nodes      = num_levels ?
                        calloc(num_levels, sizeof(Node**)) : NULL,
        .active     = num_levels ?
                        calloc(num_levels, sizeof(unsigned)) : NULL,
        .disabled   = num_levels ?
                        calloc(num_levels, sizeof(ustack)) : NULL,
        .constants  = num_constants ?
                        malloc(sizeof(Node*)*num_constants) : NULL,

        .num_constants = num_constants,
        .head = NULL,
        .num_levels = num_levels,
    };

    return tree;
}


void free_tree(MathTree* tree)
{
    if (tree == NULL)   return;

    for (unsigned level=0; level < tree->num_levels; ++level) {
        for (unsigned n=0; n < tree->active[level]; ++n) {
            free(tree->nodes[level][n]);
        }
        free(tree->nodes[level]);
        free(tree->disabled[level].data);
    }

    for (unsigned c=0; c < tree->num_constants; ++c) {
        free(tree->constants[c]);
    }

    free(tree->nodes);
    free(tree->active);
    free(tree->constants);
    free(tree->disabled);

    free(tree);
}


void print_tree(MathTree* tree)
{
    print_node(tree->head);
}


void fdprint_tree(MathTree* tree, int fd)
{
    fdprint_node(tree->head, fd);
}

unsigned count_nodes(MathTree* tree)
{
    unsigned count = tree->num_constants;
    for (unsigned level=0; level < tree->num_levels; ++level) {
        count += tree->active[level];
    }
    return count;
}

////////////////////////////////////////////////////////////////////////////////

MathTree* clone_tree(MathTree* orig)
{
    MathTree* clone = new_tree(orig->num_levels, orig->num_constants);

    // Clone the constants into the new tree.
    for (int c=0; c < orig->num_constants; ++c) {
        clone->constants[c] = clone_node(orig->constants[c]);
    }

    // Then clone all of the nodes into the new tree:

    // Iterate over levels
    for (int level=0; level < orig->num_levels; ++level) {

        int active = orig->active[level];
        clone->active[level] = active;
        if (active) {
            clone->nodes[level] = malloc(sizeof(Node*)*active);
        } else {
            clone->nodes[level] = NULL;
        }

        // Clone each node in this level and opcode.
        for (int n=0; n < active; ++n) {
            Node* clone_ = clone_node(orig->nodes[level][n]);
            clone->nodes[level][n] = clone_;
        }
    }

    clone->head = orig->head->clone_address;
    return clone;
}

////////////////////////////////////////////////////////////////////////////////

_STATIC_
void disable_node(MathTree* tree, int level, int n)
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
    ustack_increment(&tree->disabled[level]);
}


void disable_nodes_binary(MathTree* tree)
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



void disable_nodes(MathTree* tree)
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
        ustack_push(&tree->disabled[level], 0);

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


void enable_nodes(MathTree* tree)
{
    for (int level=tree->num_levels-1; level >= 0; --level) {
        tree->active[level] += ustack_pop(&tree->disabled[level]);
    }
}

uint8_t active_axes(const MathTree* const tree)
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
