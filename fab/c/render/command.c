#include <math.h>
#include <stdlib.h>

#include "render/command.h"
#include "render/tape.h"
#include "render/render.h"

#include "tree/tree.h"

static void command_texture_defaults(void)
{
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

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
    command->block_size = side / command->block_count;

    {   // Create the master texture, initializing it to all zeros
        GLuint height = side;
        if (command->node_count < side)
            height = command->node_count;

        float* atlas = malloc(sizeof(float) * height * side);
        for (GLuint i=0; i < height*side; ++i)
            atlas[i] = 0;

        glGenTextures(1, &command->atlas);
        glBindTexture(GL_TEXTURE_2D, command->atlas);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, side, command->node_count,
                     0, GL_RED, GL_FLOAT, atlas);
        command_texture_defaults();

        free(atlas);
    }

    {   // Create the swap texture
        float* swap = malloc(
                sizeof(float) * command->node_max * command->block_size);

        glGenTextures(1, &command->swap);
        glBindTexture(GL_TEXTURE_2D, command->swap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, command->block_size, command->node_max,
                     0, GL_RED, GL_FLOAT, swap);
        command_texture_defaults();

        free(swap);
    }

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
