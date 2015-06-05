#ifndef MATH_G_H
#define MATH_G_H

#include "tree/math/math_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @file tree/math/math_g.h
    @brief Functions for finding partial derivatives on multiple samples.
    @details These functions take in input arrays A and B,
    and array point count c.  Results are stored in the array R.
*/

typedef struct derivative_
{
    float v;
    float dx;
    float dy;
    float dz;
} derivative;

// Binary functions
derivative* add_g(const derivative* A, const derivative* B, derivative* R, int c);
derivative* sub_g(const derivative* A, const derivative* B, derivative* R, int c);
derivative* mul_g(const derivative* A, const derivative* B, derivative* R, int c);
derivative* div_g(const derivative* A, const derivative* B, derivative* R, int c);

derivative* min_g(const derivative* A, const derivative* B, derivative* R, int c);
derivative* max_g(const derivative* A, const derivative* B, derivative* R, int c);

derivative* pow_g(const derivative* A, const derivative* B, derivative* R, int c);

// Unary functions
derivative* abs_g(const derivative* A, derivative* R, int c);
derivative* square_g(const derivative* A, derivative* R, int c);
derivative* sqrt_g(const derivative* A, derivative* R, int c);
derivative* sin_g(const derivative* A, derivative* R, int c);
derivative* cos_g(const derivative* A, derivative* R, int c);
derivative* tan_g(const derivative* A, derivative* R, int c);
derivative* asin_g(const derivative* A, derivative* R, int c);
derivative* acos_g(const derivative* A, derivative* R, int c);
derivative* atan_g(const derivative* A, derivative* R, int c);
derivative* neg_g(const derivative* A, derivative* R, int c);
derivative* exp_g(const derivative* A, derivative* R, int c);

// Variables
derivative* X_g(const float* X, derivative* R, int c);
derivative* Y_g(const float* Y, derivative* R, int c);
derivative* Z_g(const float* Z, derivative* R, int c);

#ifdef __cplusplus
}
#endif

#endif
