#include <cstdlib>

#include "fab.h"
#include "tree/tree.h"
#include "tree/parser.h"

#include "cpp/shape.h"

MathShape::MathShape(std::string math)
    : math(math), tree(parse(math.c_str()))
{
    if (tree == NULL)
    {
        throw fab::ParseError();
    }
}

MathShape::MathShape(std::string math, float xmin, float ymin,
                     float xmax, float ymax)
    : math(math), bounds(xmin, ymin, xmax, ymax), tree(parse(math.c_str()))
{
    if (tree == NULL)
    {
        throw fab::ParseError();
    }
}

MathShape::MathShape(std::string math, float xmin, float ymin, float zmin,
                     float xmax, float ymax, float zmax)
    : math(math), bounds(xmin, ymin, zmin, xmax, ymax, zmax),  tree(parse(math.c_str()))
{
    if (tree == NULL)
    {
        throw fab::ParseError();
    }
}

MathShape::MathShape(std::string math, Bounds bounds)
    : math(math), bounds(bounds), tree(parse(math.c_str()))
{
    if (tree == NULL)
    {
        throw fab::ParseError();
    }
}

MathShape::~MathShape()
{
   free(tree);
}

MathTree* MathShape::getTree()
{
    return tree;
}

MathShape MathShape::map(Transform t) const
{
    return MathShape("m" + (t.x_forward.length() ? t.x_forward : " ")
                         + (t.y_forward.length() ? t.y_forward : " ")
                         + (t.z_forward.length() ? t.z_reverse : " ") + math,
                     bounds.map(t));
}
