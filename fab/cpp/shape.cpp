#include <cstdlib>
#include <cmath>

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

MathShape operator~(const MathShape& a)
{
    return MathShape("n" + a.math);
}


MathShape operator&(const MathShape& a, const MathShape& b)
{
    return MathShape("a" + a.math + b.math,
                     fmax(a.bounds.xmin, b.bounds.xmin),
                     fmax(a.bounds.ymin, b.bounds.ymin),
                     fmax(a.bounds.zmin, b.bounds.zmin),
                     fmin(a.bounds.xmax, b.bounds.xmax),
                     fmin(a.bounds.ymax, b.bounds.ymax),
                     fmin(a.bounds.zmax, b.bounds.zmax));
}

MathShape operator|(const MathShape& a, const MathShape& b)
{
    return MathShape("i" + a.math + b.math,
                     fmin(a.bounds.xmin, b.bounds.xmin),
                     fmin(a.bounds.ymin, b.bounds.ymin),
                     fmin(a.bounds.zmin, b.bounds.zmin),
                     fmax(a.bounds.xmax, b.bounds.xmax),
                     fmax(a.bounds.ymax, b.bounds.ymax),
                     fmax(a.bounds.zmax, b.bounds.zmax));

}
