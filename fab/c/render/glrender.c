#include <stdio.h>
#include <stdlib.h>

#include "tree/tree.h"
#include "tree/parser.h"

#include "render/render.h"
#include "render/command.h"

int main(int argc, char** argv)
{
    gl_init("/Users/mkeeter/code/antimony/fab/c/render/shaders/");

    MathTree* tree = parse("-Xf1");

    RenderCommand* command = command_init(tree);
    float* xyz = malloc(sizeof(float)*command->block_size);
    printf("Block size:  %u\n", command->block_size);
    printf("Block count: %u\n", command->block_count);
    printf("Node count:  %u\n", command->node_count);
    printf("Node max:    %u\n", command->node_max);
    for (int i=0; i < command->block_size; ++i)
    {
        xyz[i] = 0.1 * i;
    }

    render_command(command, xyz);

}
