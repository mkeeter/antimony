#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Eigen/Dense"

typedef Eigen::Vector3d Vec3f;

struct Triangle {
    Triangle() {}

    Triangle(Vec3f a, Vec3f b, Vec3f c) :
        a(a), b(b), c(c) {}

    /*
     *  Calculate the normal of this triangle.
     */
    Vec3f normal() const;

    Vec3f a;
    Vec3f b;
    Vec3f c;
};

#endif // TRIANGLE_H
