#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Eigen/Dense"

#include <array>

typedef Eigen::Vector3d Vec3f;

struct Triangle {
    Triangle() {}

    Triangle(Vec3f a, Vec3f b, Vec3f c) :
        a(a), b(b), c(c) {}

    std::array<float, 3> a_() const;
    std::array<float, 3> b_() const;
    std::array<float, 3> c_() const;

    std::array<float, 6> ab_() const;
    std::array<float, 6> bc_() const;
    std::array<float, 6> ca_() const;

    std::array<float, 6> ba_() const;
    std::array<float, 6> cb_() const;
    std::array<float, 6> ac_() const;

    std::array<float, 9> abc_() const;

    /*
     *  Calculate the normal of this triangle.
     */
    Vec3f normal() const;

    Vec3f a;
    Vec3f b;
    Vec3f c;
};

#endif // TRIANGLE_H
