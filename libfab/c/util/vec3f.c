#include <math.h>

#include "util/vec3f.h"

// Take coordinates in the cell's frame and project them into the view frame.
Vec3f project(const Vec3f p, const float M[4])
{
    // M = {cos a, sin a, cos b, sin b}

    // Spin around the Z axis
    float x_ = M[0]*p.x - M[1]*p.y,
          y_ = M[1]*p.x + M[0]*p.y,
          z_ = p.z;

    // Then spin around the X axis
    return (Vec3f){ .x = x_,
                    .y = M[2]*y_ + M[3]*z_,
                    .z = -M[3]*y_ + M[2]*z_};
}

////////////////////////////////////////////////////////////////////////////////

void project_cube(const Interval X, const Interval Y, const Interval Z,
                  Interval* X_, Interval* Y_, Interval* Z_,
                  const float M[4])
{
    Vec3f p = project( (Vec3f){X.lower, Y.lower, Z.lower}, M);
    *X_ = (Interval){ p.x, p.x };
    *Y_ = (Interval){ p.y, p.y };
    *Z_ = (Interval){ p.z, p.z };
    for (int n=1; n < 8; ++n) {
        p = project( (Vec3f){
                        (n & 4) ? X.upper : X.lower,
                        (n & 2) ? Y.upper : Y.lower,
                        (n & 1) ? Z.upper : Z.lower}, M);
        if (p.x < X_->lower)    X_->lower = p.x;
        if (p.x > X_->upper)    X_->upper = p.x;

        if (p.y < Y_->lower)    Y_->lower = p.y;
        if (p.y > Y_->upper)    Y_->upper = p.y;

        if (p.z < Z_->lower)    Z_->lower = p.z;
        if (p.z > Z_->upper)    Z_->upper = p.z;
    }
}

////////////////////////////////////////////////////////////////////////////////

// Take coordinates in the view frame and project them into the cell frame.
Vec3f deproject(const Vec3f p__, const float M[4])
{
    // M = {ca, sa, cb, sb}
    float x_ = p__.x,
          y_ = (M[2]*p__.y - M[3]*p__.z),
          z_ = (M[2]*p__.z + M[3]*p__.y);

    return (Vec3f){ .x = (M[0]*x_ + M[1]*y_),
                    .y = (M[0]*y_ - M[1]*x_),
                    .z = z_ };
}

////////////////////////////////////////////////////////////////////////////////

void deproject_cube(const Interval X, const Interval Y, const Interval Z,
                    Interval* X_, Interval* Y_, Interval* Z_,
                    const float M[4])
{
    Vec3f p = deproject( (Vec3f){X.lower, Y.lower, Z.lower}, M);
    *X_ = (Interval){ p.x, p.x };
    *Y_ = (Interval){ p.y, p.y };
    *Z_ = (Interval){ p.z, p.z };
    for (int n=1; n < 8; ++n) {
        p = deproject( (Vec3f){
                        (n & 4) ? X.upper : X.lower,
                        (n & 2) ? Y.upper : Y.lower,
                        (n & 1) ? Z.upper : Z.lower}, M);
        if (p.x < X_->lower)    X_->lower = p.x;
        if (p.x > X_->upper)    X_->upper = p.x;

        if (p.y < Y_->lower)    Y_->lower = p.y;
        if (p.y > Y_->upper)    Y_->upper = p.y;

        if (p.z < Z_->lower)    Z_->lower = p.z;
        if (p.z > Z_->upper)    Z_->upper = p.z;
    }
}

////////////////////////////////////////////////////////////////////////////////

Vec3f normalize(const Vec3f v)
{
    float L = sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
    return (Vec3f){ .x=v.x/L, .y=v.y/L, .z=v.z/L };
}
