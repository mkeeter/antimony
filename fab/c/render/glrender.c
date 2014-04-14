#include <stdio.h>

#include "tree/tree.h"
#include "tree/parser.h"

#include "render/render.h"
#include "render/command.h"

int main(int argc, char** argv)
{
    gl_init("/Users/mkeeter/code/antimony/fab/c/render/shaders/");

    MathTree* tree = parse("-Xf1");

    RenderCommand* command = command_init(tree);
    printf("Testing %p!\n", tree);

}
