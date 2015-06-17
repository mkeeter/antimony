#include <cstdlib>
#include <cmath>

#include "fab/types/bounds.h"
#include "fab/fab.h"

#include "tree/tree.h"
#include "tree/parser.h"
#include "tree/eval.h"
#include "util/interval.h"

Bounds::Bounds()
    : Bounds(-INFINITY, -INFINITY, -INFINITY,
              INFINITY,  INFINITY,  INFINITY)
{
    // Nothing to do here
}

Bounds::Bounds(float xmin, float ymin, float xmax, float ymax)
    : Bounds(xmin, ymin, -INFINITY, xmax, ymax, INFINITY)
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
        free_tree(tree);
    }

    if (t.y_reverse.length())
    {
        MathTree* tree = parse(t.y_reverse.c_str());
        if (tree == NULL)
        {
            throw fab::ParseError();
        }
        y_out = eval_i(tree, x, y, z);
        free_tree(tree);
    }

    if (t.z_reverse.length())
    {
        MathTree* tree = parse(t.z_reverse.c_str());
        if (tree == NULL)
        {
            throw fab::ParseError();
        }
        z_out = eval_i(tree, x, y, z);
        free_tree(tree);
    }

    return Bounds(x_out.lower, y_out.lower, z_out.lower,
                  x_out.upper, y_out.upper, z_out.upper);

}


bool Bounds::is_bounded_xy() const
{
    return !isinf(xmin) && !isinf(ymin) &&
           !isinf(xmax) && !isinf(ymax);

}

bool Bounds::is_bounded_xyz() const
{
    return !isinf(xmin) && !isinf(ymin) && !isinf(zmin) &&
           !isinf(xmax) && !isinf(ymax) && !isinf(zmax);
}
