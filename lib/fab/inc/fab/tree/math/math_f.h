#ifndef MATH_F_H
#define MATH_F_H

#include "fab/tree/math/math_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @file tree/math/math_f.h
    @brief Functions for doing math on floating-point numbers.
    @details These functions take in input floats A and B,
    and return the result of their computation.
*/

// Binary functions
float add_f(float A, float B);
float sub_f(float A, float B);
float mul_f(float A, float B);
float div_f(float A, float B);

float min_f(float A, float B);
float max_f(float A, float B);

float pow_f(float A, float B);

// Unary functions
float abs_f(float A);
float square_f(float A);
float sqrt_f(float A);
float sin_f(float A);
float cos_f(float A);
float tan_f(float A);
float asin_f(float A);
float acos_f(float A);
float atan_f(float A);
float neg_f(float A);
float exp_f(float A);

// Variables
float X_f(float X);
float Y_f(float Y);
float Z_f(float Z);

#ifdef __cplusplus
}
#endif

#endif
