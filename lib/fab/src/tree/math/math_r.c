#include <string.h>
#include <math.h>

#include "fab/tree/math/math_r.h"
#include "fab/tree/math/math_f.h"

#define DUAL(name, fn) \
float* name(const float* restrict A, const float* restrict B,   \
         float* restrict R, int c)                              \
{                                                               \
    for (int q=0; q < c; ++q)                                   \
        R[q] = fn(A[q], B[q]);                                  \
    return R; \
}

DUAL(add_r, add_f);
DUAL(sub_r, sub_f);
DUAL(mul_r, mul_f);
DUAL(div_r, div_f);
DUAL(min_r, min_f);
DUAL(max_r, max_f);
DUAL(pow_r, pow_f);
DUAL(atan2_r, atan2_f);

////////////////////////////////////////////////////////////////////////////////

#define SINGLE(name, fn) \
float* name(const float* restrict A, float* restrict R, int c)  \
{                                                               \
    for (int q=0; q < c; ++q)                                   \
        R[q] = fn(A[q]);                                        \
    return R; \
}

SINGLE(abs_r, abs_f);
SINGLE(square_r, square_f);
SINGLE(sqrt_r, sqrt_f);
SINGLE(sin_r, sin_f);
SINGLE(cos_r, cos_f);
SINGLE(tan_r, tan_f);
SINGLE(asin_r, asin_f);
SINGLE(acos_r, acos_f);
SINGLE(atan_r, atan_f);
SINGLE(neg_r, neg_f);
SINGLE(exp_r, exp_f);

////////////////////////////////////////////////////////////////////////////////

float* X_r(const float* restrict X, float* R, int c)
{
    memcpy(R, X,c*sizeof(float));
    return R;
}

float* Y_r(const float* restrict Y, float* R, int c)
{
    memcpy(R, Y, c*sizeof(float));
    return R;
}

float* Z_r(const float* restrict Z, float* R, int c)
{
    memcpy(R, Z, c*sizeof(float));
    return R;
}
