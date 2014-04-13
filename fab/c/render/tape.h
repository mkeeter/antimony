#ifndef TAPE_H
#define TAPE_H

#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

typedef struct RenderTape_ {
    GLint instructions;
    int node_count;
    struct RenderTape_* next;
} RenderTape;

struct MathTree;
RenderTape* tape_init(struct MathTree_* tree);

#endif
