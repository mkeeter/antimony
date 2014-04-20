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
        float* data = calloc(tree->active[level], 3*sizeof(float));

        // Pack nodes into this array
        for (unsigned n=0; n < tree->active[level]; ++n)
        {
            Node* node = tree->nodes[level][n];
            ((uint32_t*)(data))[3*n] = node->opcode;
            if (node->lhs)
            {
                if (node->lhs->flags & NODE_CONSTANT) {
                    ((uint32_t*)(data))[3*n] |= (1 << 8);
                    data[3*n + 1] = node->lhs->results.f;
                } else
                    ((uint32_t*)(data))[3*n + 1] = node->lhs->rank;
            }
            if (node->rhs)
            {
                if (node->rhs->flags & NODE_CONSTANT) {
                    ((uint32_t*)(data))[3*n] |= (1 << 9);
                    data[3*n + 2] = node->rhs->results.f;
                } else
                    ((uint32_t*)(data))[3*n + 2] = node->rhs->rank;
            }
            node->rank = rank++;
            printf("%f %f %f\n", data[3*n], data[3*n+1], data[3*n+2]);
        }

        printf("Tex = %i\n", tex);
        glBindTexture(GL_TEXTURE_1D, tex);
        printf("error: %i\n", glGetError());
        printf("ptr: %p\n", (void*)data);

        glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB32F, tree->active[level],
                     0, GL_RGB, GL_FLOAT, data);
        printf("error: 0x%x\n", glGetError());

        *tape_end = malloc(sizeof(RenderTape));
        **tape_end = (RenderTape) {
            .instructions = tex,
            .node_count = tree->active[level],
            .next = NULL
        };
        tape_end = &(**tape_end).next;
        free(data);
    }

    return tape_start;
}
