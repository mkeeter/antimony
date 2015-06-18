#ifndef PARSER_H
#define PARSER_H

#include "fab/tree/tree.h"
#include "fab/tree/node/node.h"
#include "fab/tree/node/opcodes.h"

#ifdef __cplusplus
extern "C" {
#endif

struct MathTree_;

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

/** @brief Parses a prefix-notation math string
    @param input A null-terminated math string
    @returns The constructed MathTree, or NULL if failed
*/
struct MathTree_* parse(const char* input);

/*  Looks up a node in the cache.  If not found, it is appended to the
 *  appropriate cache slot; if found, the original node is freed and
 *  the cached node pointer is returned.
 *
 *  The input node's children should be deduplicated and cached; we check
 *  by comparing their pointer values.  Nodes are considered equal if they
 *  have the same opcode and same child pointers, or if they are OP_CONST
 *  and have the same values.
 */
Node* get_cached_node(NodeCache* const cache, Node* const n);

#ifdef __cplusplus
}
#endif

#endif
