#include <cstdlib>
#include <cmath>

#include "cpp/bounds.h"
#include "cpp/fab.h"

#include "tree/tree.h"
#include "tree/parser.h"
#include "tree/eval.h"
#include "util/interval.h"

Bounds::Bounds()
    : xmin(-INFINITY), ymin(-INFINITY), zmin(-INFINITY),
      xmax(INFINITY),  ymax(INFINITY),  zmax(INFINITY)
{
    // Nothing to do here
}

Bounds::Bounds(float xmin, float ymin, float xmax, float ymax)
    : xmin(xmin), ymin(ymin), zmin(-INFINITY),
      xmax(xmax), ymax(ymax), zmax(INFINITY)
{
    // Nothing to do here
}

Bounds::Bounds(float xmin, float ymin, float zmin,
               float xmax, float ymax, float zmax)
    : xmin(xmin), ymin(ymin), zmin(zmin),
      xmax(xmax), ymax(ymax), zmax(zmax)
{
    // Nothing to do here
}

Bounds Bounds::map(Transform t) const
{
    Interval x { xmin, xmax };
    Interval y { ymin, ymax };
    Interval z { zmin, zmax };

    Interval x_out = x;
    Interval y_out = y;
    Interval z_out = z;

    if (t.x_reverse.length())
    {
        MathTree* tree = parse(t.x_reverse.c_str());
        if (tree == NULL)
        {
            throw fab::ParseError();
        }
        x_out = eval_i(tree, x, y, z);
        free(tree);
    }

    if (t.y_reverse.length())
    {
        MathTree* tree = parse(t.y_reverse.c_str());
        if (tree == NULL)
        {
            throw fab::ParseError();
        }
        y_out = eval_i(tree, x, y, z);
        free(tree);
    }

    if (t.z_reverse.length())
    {
        MathTree* tree = parse(t.z_reverse.c_str());
        if (tree == NULL)
        {
            throw fab::ParseError();
        }
        z_out = eval_i(tree, x, y, z);
        free(tree);
    }

    return Bounds(x_out.lower, y_out.lower, z_out.lower,
                  x_out.upper, y_out.upper, z_out.upper);

}
