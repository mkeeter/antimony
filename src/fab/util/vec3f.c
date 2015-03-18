#include "util/vec3f.h"

bool vec3f_eq(Vec3f a, Vec3f b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

float vec3f_dot(Vec3f a, Vec3f b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

Vec3f vec3f_cross(Vec3f a, Vec3f b)
{
    return (Vec3f){
        a.y*b.z - a.z*b.y,
        a.z*b.x - a.x*b.z,
        a.x*b.y - a.y*b.x};
}
