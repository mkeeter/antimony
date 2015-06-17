#ifndef PARSER_H
#define PARSER_H
#include "tree/tree.h"
#include "tree/node/node.h"
#include "tree/node/opcodes.h"

#ifdef __cplusplus
extern "C" {
#endif

struct MathTree_;

/** @brief Parses a prefix-notation math string
    @param input A null-terminated math string
    @returns The constructed MathTree, or NULL if failed
*/
struct MathTree_* parse(const char* input);

#ifdef __cplusplus
}
#endif


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

#endif
