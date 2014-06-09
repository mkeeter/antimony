#include "tree/tree.h"
#include "tree/parser.h"

#include "shape.h"

MathShape::MathShape(std::string math, float xmin, float ymin, float zmin, float xmax, float ymax, float zmax)
    : tree(NULL), math(math),
      xmin(xmin), ymin(ymin), zmin(zmin),
      xmax(xmax), ymax(ymax), zmax(zmax)
{
    // Nothing to do here
}

MathTree* MathShape::getTree()
{
    if (tree == NULL)
    {
        tree = parse(math.c_str());
    }
    return tree;
}
