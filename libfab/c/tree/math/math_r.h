#ifndef MATH_R_H
#define MATH_R_H

/** @file tree/math/math_r.h
    @brief Functions for doing math on many distinct points at once
    @details These functions take in input arrays A and B,
    and array point count c.  Results are stored in the array R.
*/

// Binary functions
float* add_r(float* A, float* B, float* R, int c);
float* sub_r(float* A, float* B, float* R, int c);
float* mul_r(float* A, float* B, float* R, int c);
float* div_r(float* A, float* B, float* R, int c);

float* min_r(float* A, float* B, float* R, int c);
float* max_r(float* A, float* B, float* R, int c);

float* pow_r(float* A, float* B, float* R, int c);

// Unary functions
float* abs_r(float* A, float* R, int c);
float* square_r(float* A, float* R, int c);
float* sqrt_r(float* A, float* R, int c);
float* sin_r(float* A, float* R, int c);
float* cos_r(float* A, float* R, int c);
float* tan_r(float* A, float* R, int c);
float* asin_r(float* A, float* R, int c);
float* acos_r(float* A, float* R, int c);
float* atan_r(float* A, float* R, int c);
float* neg_r(float* A, float* R, int c);

// Variables
float* X_r(float* X, float* R, int c);
float* Y_r(float* Y, float* R, int c);
float* Z_r(float* Z, float* R, int c);

#endif
