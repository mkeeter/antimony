#ifndef BOUNDS_H
#define BOUNDS_H

#include "transform.h"

struct Bounds
{
public:
    Bounds();
    Bounds(float xmin, float ymin, float xmax, float ymax);
    Bounds(float xmin, float ymin, float zmin,
           float xmax, float ymax, float zmax);

    /** Applies the inverse transform to bounds intervals.
     *
     *  Raises fab::ParseError if Transform contains invalid expressions.
     */
    Bounds map(Transform t) const;

    float xmin, ymin, zmin;
    float xmax, ymax, zmax;

    bool is_bounded_xy() const;
    bool is_bounded_xyz() const;
};

#endif // BOUNDS_H
