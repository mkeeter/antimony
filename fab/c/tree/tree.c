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
                        calloc(num_levels, sizeof(Node**) * LAST_OP) : NULL,
        .active     = num_levels ?
                        calloc(num_levels, sizeof(unsigned[LAST_OP])) : NULL,

        .constants = num_constants ?
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
        for (unsigned op=0; op < LAST_OP; ++op) {
            for (unsigned n=0; n < tree->active[level][op]; ++n) {
                free(tree->nodes[level][op][n]);
            }
            free(tree->nodes[level][op]);
        }
    }

    for (unsigned c=0; c < tree->num_constants; ++c) {
        free(tree->constants[c]);
    }

    free(tree->nodes);
    free(tree->active);
    free(tree->constants);

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


void print_tree_verbose(MathTree* tree)
{
    for (unsigned level=0; level < tree->num_levels; ++level) {
        printf("Level %i:\n", level);
        for (unsigned op=0; op < LAST_OP; ++op) {
            if (tree->active[level][op] == 0)   continue;
            printf("  %s:\n", OPCODE_NAMES[op]);
            for (unsigned n=0; n < tree->active[level][op]; ++n) {
                printf("    ");
                print_node(tree->nodes[level][op][n]);
                printf("\n");
            }
        }
    }

    if (tree->num_constants) {
        if (tree->num_levels) printf("\n");
        printf("Constants:\n");
        for (unsigned c=0; c < tree->num_constants; ++c) {
            printf("    ");
            print_node(tree->constants[c]);
            printf("\n");
        }
    }
}


unsigned count_nodes(MathTree* tree)
{
    unsigned count = tree->num_constants;
    for (unsigned level=0; level < tree->num_levels; ++level) {
        for (unsigned op=0; op < LAST_OP; ++op) {
            count += tree->active[level][op];
        }
    }
    return count;
}


void fdprint_tree_verbose(MathTree* tree, int fd)
{
    FILE* f = fdopen(fd, "w");

    for (unsigned level=0; level < tree->num_levels; ++level) {
        fprintf(f, "Level %i:\n", level);
        for (unsigned op=0; op < LAST_OP; ++op) {
            if (tree->active[level][op] == 0)   continue;
            fprintf(f, "  %s:\n", OPCODE_NAMES[op]);
            for (unsigned n=0; n < tree->active[level][op]; ++n) {
                fprintf(f, "    ");
                fprint_node(tree->nodes[level][op][n], f);
                fprintf(f, "\n");
            }
        }
    }

    if (tree->num_constants) {
        if (tree->num_levels) fprintf(f, "\n");
        fprintf(f, "Constants:\n");
        for (unsigned c=0; c < tree->num_constants; ++c) {
            fprintf(f, "    ");
            fprint_node(tree->constants[c], f);
            fprintf(f, "\n");
        }
    }

    fclose(f);
}


void dot_tree(const MathTree* const tree, const char* filename)
{

    FILE* dot = fopen(filename, "wb");
    fprintf(dot, "digraph math {\n");
    fprintf(dot, "node [fontsize = 14, fontname = Arial]\n");
    fprintf(dot, "edge [color=\"#00000055\"]\n");

    for (int n=0; n < tree->num_constants; ++n) {
        Node* node = tree->constants[n];
        fprintf(dot,"\"p%p\" [shape=\"rectangle\", color=\"%s\", label=\"%g\"]\n",
                (void*)node, dot_color(node->opcode),
                node->results.f);
    }

    for (int level=0; level < tree->num_levels; ++level) {
        for (int op=0; op < LAST_OP; ++op) {
            for (int n=0; n < tree->active[level][op]; ++n) {
                Node* node = tree->nodes[level][op][n];

                fprintf(dot,"\"p%p\" [shape=\"rectangle\", color=\"%s\", label=\"%s\",fontsize=%i]\n",
                        (void*)node, dot_color(op),
                        dot_symbol(op), dot_fontsize(op));


                if (node->lhs) {
                    fprintf(dot, "p%p -> p%p [color=\"%s\"];\n",
                        (void*)node, (void*)node->lhs,
                        dot_color(op));
                }
                if (node->rhs) {
                    fprintf(dot, "p%p -> p%p [color=\"%s\"];\n",
                        (void*)node, (void*)node->rhs,
                        dot_color(op));
                }
            }
        }
    }
    fprintf(dot, "}");
    fclose(dot);
}


void dot_arrays(const MathTree* const tree, const char* filename)
{
    FILE* dot = fopen(filename, "wb");
    fprintf(dot, "digraph math {\n");
    fprintf(dot, "node [rank = min, fontsize = 14, fontname = Arial, shape=plaintext]\n");
    fprintf(dot, "edge [color=\"#00000055\"]\n");

    fprintf(dot, "struct%i [label=<<TABLE BORDER=\"0\" CELLBORDER = \"1\" CELLSPACING=\"0\"><TR>\n", 0);
    for (int n=0; n < tree->num_constants; ++n) {
        fprintf(dot, "    <TD PORT=\"p%p\" COLOR=\"%s\">%g</TD>\n",
                (void*)tree->constants[n], dot_color(OP_CONST),
                tree->constants[n]->results.f);
    }
    fprintf(dot, "</TR></TABLE>>];\n");

    for (int level=0; level < tree->num_levels; ++level) {
        fprintf(dot, "struct%i [label=<<TABLE BORDER=\"0\" CELLBORDER = \"1\" CELLSPACING=\"0\"><TR>\n", level+1);

        for (int op=0; op < LAST_OP; ++op) {
            for (int n=0; n < tree->active[level][op]; ++n) {
                fprintf(dot, "    <TD PORT=\"p%p\" COLOR=\"%s\">%s</TD>\n",
                        (void*)tree->nodes[level][op][n],
                        dot_color(op),
                        dot_symbol(op));
            }
        }
        fprintf(dot, "</TR></TABLE>>];\n");
    }

    for (int level=0; level < tree->num_levels; ++level) {
        for (int op=0; op < LAST_OP; ++op) {
            for (int n=0; n < tree->active[level][op]; ++n) {
                Node* node = tree->nodes[level][op][n];
                if (node->lhs == NULL && node->rhs == NULL)  continue;
                if (node->lhs) {
                    fprintf(
                        dot, "struct%i:p%p -> struct%i:p%p  [color=\"%s\"];\n",
                        level+1, (void*)node,
                        (node->lhs->flags & NODE_CONSTANT) ?
                            0 : node->lhs->rank+1,
                        (void*)node->lhs, dot_color(op)
                    );
                }
                if (node->rhs) {
                    fprintf(
                        dot, "struct%i:p%p -> struct%i:p%p  [color=\"%s\"];\n",
                        level+1, (void*)node,
                        (node->rhs->flags & NODE_CONSTANT) ?
                            0 : node->rhs->rank+1,
                        (void*)node->rhs, dot_color(op)
                    );
                }
            }
        }
    }

    // Draw an invisible arrow to put the constants below the lowest
    // level of the tree
    if (tree->num_levels && tree->num_constants) {
        for (int op=0; op < LAST_OP; ++op) {
            if (tree->active[0][op]) {
                fprintf(dot, "struct1:p%p -> struct0:p%p [style=invis];",
                    (void*)tree->nodes[0][op][0], (void*)tree->constants[0]);
                break;
            }
        }
    }
    fprintf(dot, "}");
    fclose(dot);
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

        // Iterate over opcodes
        for (int op=0; op < LAST_OP; ++op) {

            int active = orig->active[level][op];
            clone->active[level][op] = active;
            if (active) {
                clone->nodes[level][op] = malloc(sizeof(Node*)*active);
            } else {
                clone->nodes[level][op] = NULL;
            }

            // Clone each node in this level and opcode.
            for (int n=0; n < active; ++n) {
                Node* clone_ = clone_node(orig->nodes[level][op][n]);
                clone->nodes[level][op][n] = clone_;
            }
        }
    }

    clone->head = orig->head->clone_address;
    return clone;
}
