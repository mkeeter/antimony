#ifndef VEC3F_H
#define VEC3F_H

#include "util/interval.h"

typedef struct Vec3f_ {
    float x;
    float y;
    float z;
} Vec3f;

/*  project
 *
 *  Transform from cell frame to view frame
 *
 *  M should be floats with values [cos(a), sin(a), cos(b), sin(b)]
 *  where a is rotation about the z axis and b is rotation about
 *  the new x axis
 */
Vec3f project(const Vec3f p, const float M[4]);


/*  project_cube
 *
 *  Transforms a cubical region from cell frame to view frame, storing
 *  the maximum possible bounding box in X_, Y_, and Z_ variables.
 */
void project_cube(const Interval X, const Interval Y, const Interval Z,
                  Interval* X_, Interval* Y_, Interval* Z_,
                  const float M[4]);


/*  deproject
 *
 *  Transforms coordinates from view frame to cell frame
 *  M is the same matrix as M in project.
 */
Vec3f deproject(const Vec3f p__, const float M[4]);


/*  deproject_cube
 *
 *  Transforms a cubical region from view frame to cell frame, storing
 *  the maximum possible bounding box in X_, Y_, and Z_ variables.
 */
void deproject_cube(const Interval X, const Interval Y, const Interval Z,
                     Interval* X_, Interval* Y_, Interval* Z_,
                     const float M[4]);


/*  normalize
 *
 *  Normalizes a vector so that it is 1 unit long.
 */
Vec3f normalize(const Vec3f v);


#endif
