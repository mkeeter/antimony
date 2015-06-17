#ifndef PRINTERS_H
#define PRINTERS_H

#include <stdio.h>

#define ANSI_COLOR_GRAY		"\x1b[30;1m"
#define ANSI_COLOR_RED		"\x1b[31m"
#define ANSI_COLOR_GREEN	"\x1b[32m"
#define ANSI_COLOR_YELLOW	"\x1b[33m"
#define ANSI_COLOR_BLUE		"\x1b[34m"
#define ANSI_COLOR_MAGENTA 	"\x1b[35m"
#define ANSI_COLOR_CYAN 	"\x1b[36m"
#define ANSI_COLOR_RESET 	"\x1b[0m"


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
