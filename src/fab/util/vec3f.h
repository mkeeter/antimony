#ifndef UTIL_VEC3F_H
#define UTIL_VEC3F_H

#include <stdbool.h>

#include "util/interval.h"

typedef struct Vec3f_ {
    float x;
    float y;
    float z;
} Vec3f;

bool vec3f_eq(Vec3f a, Vec3f b);

#endif
