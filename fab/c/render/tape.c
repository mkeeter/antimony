#include <stdlib.h>

#include "tree/tree.h"
#include "tree/node/node.h"
#include "render/tape.h"

RenderTape* tape_init(MathTree* tree)
{
    GLuint rank = 0;

    RenderTape* tape_start = NULL;
    RenderTape** tape_end = &tape_start;

    for (unsigned level=0; level < tree->num_levels; ++level)
    {
        GLuint tex;
        glGenTextures(1, &tex);

        // Create a data array that will be stored in this texture
        int32_t* data = calloc(tree->active[level], 3*sizeof(int32_t));

        // Pack nodes into this array
        for (unsigned n=0; n < tree->active[level]; ++n)
        {
            Node* node = tree->nodes[level][n];
            data[3*n] = node->opcode;
            if (node->lhs)
            {
                if (node->lhs->flags & NODE_CONSTANT) {
                    data[3*n] |= (1 << 8);
                    ((float*)data)[3*n + 1] = node->lhs->results.f;
                } else
                    data[3*n + 1] = node->lhs->rank;
            }
            if (node->rhs)
            {
                if (node->rhs->flags & NODE_CONSTANT) {
                    data[3*n] |= (1 << 9);
                    ((float*)data)[3*n + 2] = node->rhs->results.f;
                } else
                    data[3*n + 2] = node->rhs->rank;
            }
            node->rank = rank++;
        }

        glBindTexture(GL_TEXTURE_1D, tex);
        glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB32I, tree->active[level],
                     0, GL_RGB, GL_INT, data);

        *tape_end = malloc(sizeof(RenderTape));
        **tape_end = (RenderTape) {
            .instructions = tex,
            .node_count = tree->active[level],
            .next = NULL
        };
        tape_end = &(**tape_end).next;
    }

    return tape_start;
}
