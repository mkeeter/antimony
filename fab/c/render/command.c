#include <math.h>
#include <stdlib.h>

#include "render/command.h"
#include "render/tape.h"
#include "render/render.h"

#include "tree/tree.h"

RenderCommand* command_init(MathTree* tree)
{
    RenderCommand* command = calloc(1, sizeof(RenderCommand));
    command->tape = tape_init(tree);

    // Count number of nodes and maximum nodes in a single render pass
    RenderTape* end = command->tape;
    while (end)
    {
        command->node_count += end->node_count;
        if (end->node_count > command->node_max)
            command->node_max = end->node_count;
        end = end->next;
    }

    // Find max texture size
    glfwMakeContextCurrent(window);
    GLint side;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &side);

    // Block count is the number of xyz blocks in the texture's x space
    // It is chosen so that all of the node values can fit in the texture.
    command->block_count = ceil(command->node_max / (float)side);

    // Block size is the number of points in an xyz block
    command->block_size = 8;//side / command->block_count;

    command->atlas_cols = command->block_count * command->block_size;
    command->atlas_rows = ceil(command->node_count / (float)command->block_count);

    {   // Create the master texture, initializing it to all zeros

        float* atlas = malloc(sizeof(float) * command->atlas_rows *
                                              command->atlas_cols);
        for (GLuint i=0; i < command->atlas_rows * command->atlas_cols; ++i)
            atlas[i] = 0;

        glGenTextures(1, &command->atlas);
        glBindTexture(GL_TEXTURE_2D, command->atlas);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, command->atlas_cols,
                     command->atlas_rows, 0, GL_RED, GL_FLOAT, atlas);
        gl_tex_defaults(GL_TEXTURE_2D);

        free(atlas);
    }

    {   // Create the swap texture
        float* swap = malloc(
                sizeof(float) * command->node_max * command->block_size);

        glGenTextures(1, &command->swap);
        glBindTexture(GL_TEXTURE_2D, command->swap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, command->block_size, command->node_max,
                     0, GL_RED, GL_FLOAT, swap);
        gl_tex_defaults(GL_TEXTURE_2D);

        free(swap);
    }

    glGenTextures(1, &command->xyz);
    glBindTexture(GL_TEXTURE_1D, command->xyz);
    gl_tex_defaults(GL_TEXTURE_1D);

    // Generate miscellaneous OpenGL objects
    glGenFramebuffers(1, &command->fbo);
    glGenVertexArrays(1, &command->vao);

    // Make a screen-filling flat pane used for texture FBO rendering
    GLfloat rect[12] = {
            -1, -1,  1, -1,   1,  1,
            -1, -1,  1,  1,  -1,  1};
    glGenBuffers(1, &command->rect);
    glBindBuffer(GL_ARRAY_BUFFER, command->rect);
    glBufferData(GL_ARRAY_BUFFER, 12*sizeof(rect[0]),
                 &rect[0], GL_STATIC_DRAW);

    return command;
}
