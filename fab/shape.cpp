#include <cstdlib>

#include "tree/tree.h"
#include "tree/parser.h"

#include "shape.h"

MathShape::MathShape(std::string math)
    : tree(NULL), math(math)
{
    // Nothing to do here
}

MathShape::MathShape(std::string math, float xmin, float ymin,
                     float xmax, float ymax)
    : tree(NULL), math(math), bounds(xmin, ymin, xmax, ymax)
{
    // Nothing to do here
}

MathShape::MathShape(std::string math, float xmin, float ymin, float zmin,
                     float xmax, float ymax, float zmax)
    : tree(NULL), math(math), bounds(xmin, ymin, zmin, xmax, ymax, zmax)
{
    // Nothing to do here
}

MathShape::MathShape(std::string math, Bounds bounds)
    : tree(NULL), math(math), bounds(bounds)
{
    // Nothing to do here
}

MathShape::~MathShape()
{
   free(tree);
}

MathTree* MathShape::getTree()
{
    if (tree == NULL)
    {
        tree = parse(math.c_str());
    }
    return tree;
}


MathShape MathShape::map(Transform t) const
{
    return MathShape("m" + (t.x_forward.length() ? t.x_forward : " ")
                         + (t.y_forward.length() ? t.y_forward : " ")
                         + (t.z_forward.length() ? t.z_reverse : " ") + math,
                     bounds.map(t));
}
