#include "tree/triangulate/triangle.h"

std::array<float, 3> Triangle::a_() const
{
    return {{static_cast<float>(a[0]),
             static_cast<float>(a[1]),
             static_cast<float>(a[2])}};
}

std::array<float, 3> Triangle::b_() const
{
    return {{static_cast<float>(b[0]),
             static_cast<float>(b[1]),
             static_cast<float>(b[2])}};
}

std::array<float, 3> Triangle::c_() const
{
    return {{static_cast<float>(c[0]),
             static_cast<float>(c[1]),
             static_cast<float>(c[2])}};
}

Vec3f Triangle::normal() const
{
    return (b - a).cross(c - a);
}
