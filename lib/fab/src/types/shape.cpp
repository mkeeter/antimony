#include <boost/python.hpp>

#include <cstdlib>
#include <cmath>

#include "fab/fab.h"
#include "fab/tree/tree.h"
#include "fab/tree/parser.h"

#include "fab/types/shape.h"

using namespace boost::python;

Shape::Shape(std::string math, Bounds bounds, int r, int g, int b)
    : Shape(math, bounds, int3(r,g,b))
{
    // Nothing to do here
}

Shape::Shape(std::string math, Bounds bounds, int3 color)
    : math(math), bounds(bounds), tree(parse(math.c_str()), free_tree),
      r(std::get<0>(color)), g(std::get<1>(color)), b(std::get<2>(color))
{
    if (tree == NULL)
        throw fab::ParseError();
}

object Shape::init(tuple args, dict kwargs)
{
    object shape = args[0];
    args = tuple(args.slice(1,_));

    std::string math = "=1;";
    Bounds bounds;
    int r = -1, g = -1, b = -1;

    // If there's a first argument, it should be either a math string or None
    if (len(args) >= 1)
    {
        object a0 = args[0];
        extract<std::string> e(a0);
        if (e.check())
            math = e();
        else if (!a0.is_none())
            throw fab::ShapeError(); //"First argument must be a string (or None)");
    }

    // string, bounds
    if (len(args) == 2)
    {
        extract<Bounds> e(args[1]);
        if (e.check())
            bounds = e();
        else
            throw fab::ShapeError(); // Second argument must be Bounds
    }

    // string, xmin, ymin, xmax, ymax
    if (len(args) == 5)
    {
        float p[4];
        for (int i=0; i < 4; ++i)
        {
            extract<float> e(args[i+1]);
            if (e.check())
                p[i] = e();
            else
                throw fab::ShapeError(); // Arguments must be float
        }
        bounds = Bounds(p[0], p[1], p[2], p[3]);
    }

    // string, xmin, ymin, zmin, xmax, ymax, zmax
    if (len(args) == 7)
    {
        float p[6];
        for (int i=0; i < 6; ++i)
        {
            extract<float> e(args[i+1]);
            if (e.check())
                p[i] = e();
            else
                throw fab::ShapeError(); // Arguments must be float
            bounds = Bounds(p[0], p[1], p[2], p[3], p[4], p[5]);
        }
    }

    // Extract optional color kwarg
    if (kwargs.has_key("color"))
    {
        extract<tuple> e(kwargs["color"]);
        if (!e.check())
            throw fab::ShapeError(); // color must be a tuple
        tuple t = e();

        if (len(t) != 3)
            throw fab::ShapeError(); // color must have 3 arguments

        int c[3];
        for (int i=0; i < 3; ++i)
        {
            extract<int> e(t[i]);
            if (e.check())
                c[i] = e();
            else
                throw fab::ShapeError(); // Arguments must be float
        }
        r = c[0];
        g = c[1];
        b = c[2];
    }

    return shape.attr("__init__")(math, bounds, r, g, b);
}

std::string Shape::repr() const
{
    std::string out = "fab.types.Shape('" + math + "'";

    // Special case for infinite (default) bounds
    if (bounds.xmin != -INFINITY || bounds.xmax != INFINITY ||
        bounds.ymin != -INFINITY || bounds.ymax != INFINITY ||
        bounds.zmin != -INFINITY || bounds.zmax != INFINITY)
    {
        out +=
           ",float('" + std::to_string(bounds.xmin) + "')," +
            "float('" +std::to_string(bounds.ymin) + "')," +
            "float('" +std::to_string(bounds.zmin) + "')," +
            "float('" +std::to_string(bounds.xmax) + "')," +
            "float('" +std::to_string(bounds.ymax) + "')," +
            "float('" +std::to_string(bounds.zmax) + "')";
    }

    if (r != -1 || g != -1 || b != -1)
        out += ",color=(" + std::to_string(r) + "," +
                            std::to_string(g) + "," +
                            std::to_string(b) + ")";

    return out + ")";
}

Shape Shape::map(Transform t) const
{
    return Shape("m" + (t.x_forward.length() ? t.x_forward : "_")
                     + (t.y_forward.length() ? t.y_forward : "_")
                     + (t.z_forward.length() ? t.z_forward : "_") + math,
                 bounds.map(t));
}

Shape operator~(const Shape& a)
{
    return Shape("n" + a.math);
}


Shape operator&(const Shape& a, const Shape& b)
{
    return Shape("a" + a.math + b.math, Bounds(
                     fmax(a.bounds.xmin, b.bounds.xmin),
                     fmax(a.bounds.ymin, b.bounds.ymin),
                     fmax(a.bounds.zmin, b.bounds.zmin),
                     fmin(a.bounds.xmax, b.bounds.xmax),
                     fmin(a.bounds.ymax, b.bounds.ymax),
                     fmin(a.bounds.zmax, b.bounds.zmax)));
}

Shape operator|(const Shape& a, const Shape& b)
{
    return Shape("i" + a.math + b.math, Bounds(
                     fmin(a.bounds.xmin, b.bounds.xmin),
                     fmin(a.bounds.ymin, b.bounds.ymin),
                     fmin(a.bounds.zmin, b.bounds.zmin),
                     fmax(a.bounds.xmax, b.bounds.xmax),
                     fmax(a.bounds.ymax, b.bounds.ymax),
                     fmax(a.bounds.zmax, b.bounds.zmax)));

}
