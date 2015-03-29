#include "tree/triangulate/triangle.h"

Vec3f Triangle::normal() const
{
    return (b - a).cross(c - a);
}
