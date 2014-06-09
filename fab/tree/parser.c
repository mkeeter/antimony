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

#include "tree/tree.h"
#include "tree/parser.h"

#include "tree/node/node.h"
#include "tree/node/opcodes.h"

////////////////////////////////////////////////////////////////////////////////

/* Linked list of Nodes */
typedef struct NodeList_
{
    Node* node;
    struct NodeList_* next;
} NodeList;


/* Cache storing multiple lists of NodeLists */
typedef struct NodeCache_
{
    int levels;
    NodeList* (*nodes)[LAST_OP]; // indexed by [level][opcode]
    NodeList* constants;
} NodeCache;


/** @brief Recursively sets the flag of nodes to contain NODE_IN_TREE */
_STATIC_
void flag_in_tree(Node* n);

/** @brief Returns the next token (recursively) in the input stream,
    @param input Input stream, incremented as we go
    @param failed Flag set if we fail
    @param X Node to use for 'X' token
    @param Y Node to use for 'Y' token
    @param Z Node to use for 'Z' token
    @param cache Node cache
*/
_STATIC_
Node* get_token(const char** input, _Bool* const failed,
                Node* X, Node* Y, Node* Z, NodeCache* const cache);

/** @brief Gets a float from the input stream
    @param input Input stream (incremented as we go)
    @param failed Flag (set to True if something goes wrong)
    @returns An OP_CONST node wrapping the float
*/
_STATIC_
Node* get_float(const char** input, _Bool* const failed);


/*  Looks up a node in the cache.  If not found, it is appended to the
 *  appropriate cache slot; if found, the original node is freed and
 *  the cached node pointer is returned.
 *
 *  The input node's children should be deduplicated and cached; we check
 *  by comparing their pointer values.  Nodes are considered equal if they
 *  have the same opcode and same child pointers, or if they are OP_CONST
 *  and have the same values.
 */
_STATIC_
Node* get_cached_node(NodeCache* const cache, Node* const n);

/*  Counts the number of nodes in this list with NODE_IN_TREE in their flags
 */
_STATIC_
unsigned count_list_nodes(NodeList* list);

/* Destructively converts a list into an array of nodes, only copying nodes
 * with NODE_IN_TREE set in their flags.
 */
_STATIC_
unsigned flatten_list(NodeList* list, Node** array);

/*  Destructively loads the cache into a tree,
    freeing linked lists as we go.  Returns number of nodes copied.
*/
_STATIC_
struct MathTree_* cache_to_tree(NodeCache* c);


/** @brief Frees a NodeCache */
_STATIC_
void free_node_cache(NodeCache* const c);

////////////////////////////////////////////////////////////////////////////////


MathTree* parse(const char* input)
{
    _Bool failed = false;

    // Create a cache in which nodes will be stored
    NodeCache* cache = malloc(sizeof(NodeCache));
    *cache = (NodeCache){ .levels=0, .constants=NULL };

    // Throw X, Y, and Z nodes into the cache
    Node* X = get_cached_node(cache, X_n());
    Node* Y = get_cached_node(cache, Y_n());
    Node* Z = get_cached_node(cache, Z_n());

    // Parse the string, storing nodes in the cache and receiving the head
    Node* head = get_token(&input, &failed, X, Y, Z, cache);

    //  Abort if:
    //      The parser failed
    //      The parser didn't return a valid head
    //          (this might be a subset of the above)
    //      The input stream isn't empty
    if (failed || !head || *input) {
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

_STATIC_
void flag_in_tree(Node* n)
{
    if (n == NULL)  return;
    if (n->flags & NODE_IN_TREE)    return;

    n->flags |= NODE_IN_TREE;
    flag_in_tree(n->lhs);
    flag_in_tree(n->rhs);
}


_STATIC_
Node* get_token(const char** const input, _Bool* const failed,
                Node* X, Node* Y, Node* Z,
                NodeCache* const cache)
{
    Node *lhs = NULL, *rhs = NULL, *out = NULL;
    Node *X_ = NULL, *Y_ = NULL, *Z_ = NULL;

    char c = *((*input)++);

    if (c == 0) {
        *failed = true;
        return NULL;
    }

    switch(c) {
        case ' ':   return NULL;

        case 'X':   out = X; break;
        case 'Y':   out = Y; break;
        case 'Z':   out = Z; break;
        case 'f':   out = get_float(input, failed); break;

        case 'm':
            X_ = get_token(input, failed, X, Y, Z, cache);
            Y_ = get_token(input, failed, X, Y, Z, cache);
            Z_ = get_token(input, failed, X, Y, Z, cache);
            out = get_token(input, failed,
                            X_ ? X_ : X,
                            Y_ ? Y_ : Y,
                            Z_ ? Z_ : Z,
                            cache);
            break;

        case '+':
        case '-':
        case '*':
        case '/':
        case 'i':
        case 'a':
        case 'p':
            lhs = get_token(input, failed, X, Y, Z, cache);
            rhs = get_token(input, failed, X, Y, Z, cache);
            break;

        case 's':
        case 'c':
        case 't':
        case 'S':
        case 'C':
        case 'T':
        case 'b':
        case 'q':
        case 'r':
        case 'n':
        case 'x':
            lhs = get_token(input, failed, X, Y, Z, cache);
            break;

        default:
            *failed = true;
    };

    if (*failed)    c = 0;

    switch(c) {

        case 'X':
        case 'Y':
        case 'Z':
        case 'f':
        case 'm':   break;

        case '+':   out = add_n(lhs, rhs); break;
        case '-':   out = sub_n(lhs, rhs); break;
        case '*':   out = mul_n(lhs, rhs); break;
        case '/':   out = div_n(lhs, rhs); break;
        case 'i':   out = min_n(lhs, rhs); break;
        case 'a':   out = max_n(lhs, rhs); break;
        case 'p':   out = pow_n(lhs, rhs); break;

        case 's':   out = sin_n(lhs); break;
        case 'c':   out = cos_n(lhs); break;
        case 't':   out = tan_n(lhs); break;
        case 'S':   out = asin_n(lhs); break;
        case 'C':   out = acos_n(lhs); break;
        case 'T':   out = atan_n(lhs); break;
        case 'b':   out = abs_n(lhs); break;
        case 'q':   out = square_n(lhs); break;
        case 'r':   out = sqrt_n(lhs); break;
        case 'n':   out = neg_n(lhs); break;
        case 'x':   out = exp_n(lhs); break;

        default:
            *failed = true;
    };

    return get_cached_node(cache, out);
}


_STATIC_
Node* get_float(const char** const input, _Bool* const failed)
{
    // Accumulated value
    float v = 0;
    _Bool neg = false;
    float divider = 0;

    if (**input == 0) {
        *failed = true;
        return NULL;
    }

    if (**input == '-') {
        neg = true;
        (*input)++;
    }

    while ((**input >= '0' && **input <= '9') || **input == '.') {
        if (**input == '.') {
            divider = 10;
        } else if (divider) {
            v += (**input - '0') / divider;
            divider *= 10;
        } else {
            v = v * 10 + (**input - '0');
        }
        (*input)++;
    }

    // Check for scientific notation.
    if (**input == 'e') {
        int e = 0;
        _Bool neg_exp = false;
        (*input)++;
        if (**input == '-') {
            neg_exp = true;
            (*input)++;
        } else if (**input == '+') {
            neg_exp = false;
            (*input)++;
        }
        do {
            e = e * 10 + (*((*input)++) - '0');
        } while (**input >= '0' && **input <= '9');

        // Negate the exponent if needed.
        e = e * (neg_exp ? -1 : 1);
        v = v * pow(10, e);
    }

    v = neg ? -v : v;

    return constant_n(v);
}


_STATIC_
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

_STATIC_
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

_STATIC_
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

_STATIC_
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

_STATIC_
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


_STATIC_
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
