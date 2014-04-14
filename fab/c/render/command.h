#ifndef COMMAND_H
#define COMMAND_H

#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

struct RenderTape_;
struct MathTree_;

typedef struct RenderCommand_ {
    GLuint fbo;     // Framebuffer object
    GLuint vao;     // Vertex array object
    GLuint rect;    // Vertex buffer for flat panel

    GLuint block_size;  // number of xyz points per block
    GLuint block_count; // number of blocks per row

    GLuint node_count;  // total number of nodes in the expression
    GLuint node_max;   // maximum number of nodes in a single evaluation

    GLuint atlas; // texture storing node values
    GLuint swap;  // texture used as render target
    struct RenderTape_* tape;
} RenderCommand;

RenderCommand* command_init(struct MathTree_* tree);

#endif
