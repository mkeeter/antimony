#ifndef PARSER_H
#define PARSER_H

struct MathTree_;

/** @brief Parses a prefix-notation math string
    @param input A null-terminated math string
    @returns The constructed MathTree, or NULL if failed
*/
struct MathTree_* parse(const char* input);

#endif
