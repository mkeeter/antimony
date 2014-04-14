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
    float* xyz = malloc(sizeof(float)*command->block_size*3);
    for (int i=0; i < command->block_size * 3; ++i)
    {
        xyz[i] = 0.1;
    }

    render_command(command, xyz);

}
