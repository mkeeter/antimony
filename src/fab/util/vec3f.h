#ifndef UTIL_VEC3F_H
#define UTIL_VEC3F_H

#include <stdbool.h>

#include "util/interval.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Vec3f_ {
    float x;
    float y;
    float z;
} Vec3f;

bool vec3f_eq(Vec3f a, Vec3f b);
float vec3f_dot(Vec3f a, Vec3f );

#ifdef __cplusplus
}
#endif

#endif
