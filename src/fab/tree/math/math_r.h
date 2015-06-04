#ifndef MATH_R_H
#define MATH_R_H

#include "tree/math/math_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @file tree/math/math_r.h
    @brief Functions for doing math on many distinct points at once
    @details These functions take in input arrays A and B,
    and array point count c.  Results are stored in the array R.
*/

// Binary functions
float* add_r(const float* A, const float* B, float* R, int c);
float* sub_r(const float* A, const float* B, float* R, int c);
float* mul_r(const float* A, const float* B, float* R, int c);
float* div_r(const float* A, const float* B, float* R, int c);

float* min_r(const float* A, const float* B, float* R, int c);
float* max_r(const float* A, const float* B, float* R, int c);

float* pow_r(const float* A, const float* B, float* R, int c);

// Unary functions
float* abs_r(const float* A, float* R, int c);
float* square_r(const float* A, float* R, int c);
float* sqrt_r(const float* A, float* R, int c);
float* sin_r(const float* A, float* R, int c);
float* cos_r(const float* A, float* R, int c);
float* tan_r(const float* A, float* R, int c);
float* asin_r(const float* A, float* R, int c);
float* acos_r(const float* A, float* R, int c);
float* atan_r(const float* A, float* R, int c);
float* neg_r(const float* A, float* R, int c);
float* exp_r(const float* A, float* R, int c);

// Variables
float* X_r(const float* X, float* R, int c);
float* Y_r(const float* Y, float* R, int c);
float* Z_r(const float* Z, float* R, int c);

#ifdef __cplusplus
}
#endif

#endif
