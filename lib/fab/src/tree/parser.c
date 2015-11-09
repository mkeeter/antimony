/*  Prefix notation math string using the following characters
    OP_ADD  +
    OP_SUB  -
    OP_MUL  *
    OP_DIV  /
    OP_MIN  i
    OP_MAX  a
    OP_POW  p

    OP_SIN  s
    OP_COS  c
    OP_TAN  t
    OP_ASIN S
    OP_ACOS C
    OP_ATAN T

    OP_ABS  b
    OP_SQUARE q
    OP_SQRT r
    OP_NEG  n
    OP_EXP  x

    OP_X    X
    OP_Y    Y
    OP_Z    Z
    OP_CONST    f (followed by value)
*/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

#include "fab/tree/tree.h"
#include "fab/tree/parser.h"
#include "fab/tree/v2parser.h"

#include "fab/tree/node/node.h"
#include "fab/tree/node/opcodes.h"

////////////////////////////////////////////////////////////////////////////////


/** @brief Recursively sets the flag of nodes to contain NODE_IN_TREE */
static
void flag_in_tree(Node* n);

/*  Counts the number of nodes in this list with NODE_IN_TREE in their flags
 */
static
unsigned count_list_nodes(NodeList* list);

/* Destructively converts a list into an array of nodes, only copying nodes
 * with NODE_IN_TREE set in their flags.
 */
static
unsigned flatten_list(NodeList* list, Node** array);

/*  Destructively loads the cache into a tree,
    freeing linked lists as we go.  Returns number of nodes copied.
*/
static
struct MathTree_* cache_to_tree(NodeCache* c);


/** @brief Frees a NodeCache */
static
void free_node_cache(NodeCache* const c);

////////////////////////////////////////////////////////////////////////////////


MathTree* parse(const char* input)
{
    // Create a cache in which nodes will be stored
    NodeCache* cache = malloc(sizeof(NodeCache));
    *cache = (NodeCache){ .levels=0, .constants=NULL };
    Node *head = NULL;

    // Throw X, Y, and Z nodes into the cache
    Node* X = get_cached_node(cache, X_n());
    Node* Y = get_cached_node(cache, Y_n());
    Node* Z = get_cached_node(cache, Z_n());

    // Parse the string, storing nodes in the cache and receiving the head
    bool success = v2parse(&head, input, X, Y, Z, cache);

    if (!success) {
        free_node_cache(cache);
        return NULL;
    }

    // Pack the cache into a MathTree data structure
    flag_in_tree(head);
    MathTree* T = cache_to_tree(cache);
    T->head = head;

    free_node_cache(cache);
    return T;
}

static
void flag_in_tree(Node* n)
{
    if (n == NULL)  return;
    if (n->flags & NODE_IN_TREE)    return;

    n->flags |= NODE_IN_TREE;
    flag_in_tree(n->lhs);
    flag_in_tree(n->rhs);
}


Node* get_cached_node(NodeCache* const cache, Node* const n)
{
    if (n == NULL)  return NULL;

    // Special case for constants: search through the constants
    // list for any that match the target node's value.
    if (n->flags & NODE_CONSTANT) {
        NodeList** next = &(cache->constants);
        while (*next) {
            if ((**next).node->results.f == n->results.f) {
                // Only free this node if it isn't the same as the match
                if (n != (**next).node) free(n);
                return (**next).node;
            }
            next = &(**next).next;
        }
        // If we didn't find it, then allocate a new node at the
        // end of the linked list and return it.
        *next  = malloc(sizeof(NodeList));
        **next = (NodeList){ .node=n, .next=NULL };
        return n;
    }

    // Expand the cache if we don't have enough slots
    if (n->rank >= cache->levels) {
        cache->nodes = realloc(cache->nodes,
                                (n->rank+1)*sizeof(NodeList*)*LAST_OP);
        // And NULL out any cache levels that are empty.
        for (int i=cache->levels; i <= n->rank; ++i) {
            for (int o=0; o < LAST_OP; ++o) {
                cache->nodes[i][o] = NULL;
            }
        }
        cache->levels = n->rank + 1;
    }

    // Fly through the linked list, comparing child pointers
    NodeList** next = &(cache->nodes[n->rank][n->opcode]);
    while (*next) {
        // If we've found a match, free the target and return the match
        if ((**next).node->lhs == n->lhs && (**next).node->rhs == n->rhs) {

            // Only free this node if it isn't the same as the match.
            if (n != (**next).node) free(n);
            return (**next).node;
        }
        next = &(**next).next;
    }
    // If we've made it to the end of the list, then extend the list
    // with the provided node.
    *next  = malloc(sizeof(NodeList));
    **next = (NodeList){ .node=n, .next=NULL };

    return n;
}

static
unsigned count_list_nodes(NodeList* list)
{
    unsigned count = 0;
    while (list) {
        if (list->node->flags & NODE_IN_TREE) {
            ++count;
        }
        list = list->next;
    }
    return count;
}

static
unsigned flatten_list(NodeList* list, Node** array)
{
    unsigned count = 0;
    while (list) {
        if (list->node->flags & NODE_IN_TREE) {
            ++count;
            *(array++) = list->node;
        } else {
            free(list->node);
        }
        NodeList* prev = list;
        list = list->next;
        free(prev);
    }
    return count;
}

static
void free_list(NodeList* list)
{
    while (list)
    {
        free(list->node);
        NodeList* prev = list;
        list = list->next;
        free(prev);
    }
}

static
MathTree* cache_to_tree(NodeCache* c)
{
    // Count the number of constants in the tree
    const unsigned num_constants = count_list_nodes(c->constants);

    // Create the tree
    MathTree* const tree = new_tree(c->levels, num_constants);

    // Copy over the constant nodes
    flatten_list(c->constants, tree->constants);
    c->constants = NULL;

    for (int level=0; level < c->levels; level++) {

        // Count up all of the nodes at this level
        {
            unsigned count = 0;
            for (int op=0; op < LAST_OP; ++op) {
                count += count_list_nodes(c->nodes[level][op]);
            }

            // Allocate space in the tree for these nodes
            tree->nodes[level] = malloc(count*sizeof(Node*));
        }

        Node** index = tree->nodes[level];
        for (int op=0; op < LAST_OP; ++op) {
            const unsigned count = flatten_list(
                    c->nodes[level][op], index);
            index += count;
            tree->active[level] += count;
            c->nodes[level][op] = NULL;
        }
    }

    return tree;
}


static
void free_node_cache(NodeCache* const c)
{
    // Free the linked list of constants
    free_list(c->constants);

    // Free the linked lists of nodes
    for (int level=0; level < c->levels; level++) {
        for (int op=0; op < LAST_OP; ++op) {
            free_list(c->nodes[level][op]);
        }
    }

    free(c->nodes);
    free(c);
}
