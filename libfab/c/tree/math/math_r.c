#include <string.h>
#include <math.h>

#include "tree/math/math_r.h"

float* add_r(float* A, float* B, float* R, int c)
{
    for (int q = 0; q < c; ++q)
        R[q] = A[q] + B[q];
    return R;
}

float* sub_r(float* A, float* B, float* R, int c)
{
    for (int q = 0; q < c; ++q)
        R[q] = A[q] - B[q];
    return R;
}

float* mul_r(float* A, float* B, float* R, int c)
{
    for (int q = 0; q < c; ++q)
        R[q] = A[q] * B[q];
    return R;
}

float* div_r(float* A, float* B, float* R, int c)
{
    for (int q = 0; q < c; ++q)
        R[q] = A[q] / B[q];
    return R;
}

float* min_r(float* A, float* B, float* R, int c)
{
    for (int q = 0; q < c; ++q)
        R[q] = fmin(A[q], B[q]);
    return R;
}

float* max_r(float* A, float* B, float* R, int c)
{
    for (int q = 0; q < c; ++q)
        R[q] = fmax(A[q], B[q]);
    return R;
}

float* pow_r(float* A, float* B, float* R, int c)
{
    for (int q = 0; q < c; ++q)
        R[q] = pow(A[q], B[q]);
    return R;
}

////////////////////////////////////////////////////////////////////////////////

float* abs_r(float* A, float* R, int c)
{
    for (int q=0; q < c; ++q)
        R[q] = fabs(A[q]);
    return R;
}

float* square_r(float* A, float* R, int c)
{
    for (int q = 0; q < c; ++q)
        R[q] = A[q]*A[q];
    return R;
}

float* sqrt_r(float* A, float* R, int c)
{
    for (int q = 0; q < c; ++q)
        if (A[q] < 0)   R[q] = 0;
        else            R[q] = sqrt(A[q]);
    return R;
}

float* sin_r(float* A, float* R, int c)
{
    for (int q = 0; q < c; ++q)
        R[q] = sin(A[q]);
    return R;
}

float* cos_r(float* A, float* R, int c)
{
    for (int q = 0; q < c; ++q)
        R[q] = cos(A[q]);
    return R;
}

float* tan_r(float* A, float* R, int c)
{
    for (int q = 0; q < c; ++q)
        R[q] = tan(A[q]);
    return R;
}

float* asin_r(float* A, float* R, int c)
{
    for (int q = 0; q < c; ++q)
        R[q] = asin(A[q]);
    return R;
}

float* acos_r(float* A, float* R, int c)
{
    for (int q = 0; q < c; ++q) {
        if (A[q] < -1)      R[q] = M_PI;
        else if (A[q] > 1)  R[q] = 0;
        else                R[q] = acos(A[q]);
    }
    return R;
}

float* atan_r(float* A, float* R, int c)
{
    for (int q = 0; q < c; ++q)
        R[q] = atan(A[q]);
    return R;
}

float* neg_r(float* A, float* R, int c)
{
    for (int q = 0; q < c; ++q)
        R[q] = -A[q];
    return R;
}

////////////////////////////////////////////////////////////////////////////////

float* X_r(float* X, float* R, int c)
{
    memcpy(R, X,c*sizeof(float));
    return R;
}

float* Y_r(float* Y, float* R, int c)
{
    memcpy(R, Y, c*sizeof(float));
    return R;
}

float* Z_r(float* Z, float* R, int c)
{
    memcpy(R, Z, c*sizeof(float));
    return R;
}
