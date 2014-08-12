#ifndef PRINTERS_H
#define PRINTERS_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Node_;

/** @brief Recursively prints a node to standard output. */
void print_node(struct Node_* n);


/** @brief Recursively prints a node to a given file descriptor */
void fdprint_node(struct Node_* n, int fd);


/** @brief Recursively prints a node to a given file */
void fprint_node(struct Node_* n, FILE* file);

#ifdef __cplusplus
}
#endif

#endif
