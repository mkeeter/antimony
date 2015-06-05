#include <string.h>
#include <math.h>

#include "tree/math/math_g.h"

float* add_g(const float (*A)[4], const float (*B)[4],
             float (*R)[4], int c)
{
    for (int q = 0; q < c; ++q)
        for (int i=0; i < 4; ++i)
            R[q][i] = A[q][i] + B[q][i];
    return R;
}

float* sub_g(const float (*A)[4], const float (*B)[4],
             float (*R)[4], int c)
{
    for (int q = 0; q < c; ++q)
    {
        for (int i=0; i < 4; ++i)
            R[q][i] = A[q][i] - B[q][i];
    }
    return R;
}

float* mul_g(const float (*A)[4], const float (*B)[4],
             float (*R)[4], int c)
{
    for (int q = 0; q < c; ++q)
    {
        R[q][0] = A[q][0] * B[q][0];

        // Product rule
        R[q][1] = A[q][0] * B[q][1] + A[q][1] * B[q][0];
        R[q][2] = A[q][0] * B[q][2] + A[q][2] * B[q][0];
        R[q][3] = A[q][0] * B[q][3] + A[q][3] * B[q][0];
    }
    return R;
}

float* div_g(const float (*A)[4], const float (*B)[4],
             float (*R)[4], int c)
{
    for (int q = 0; q < c; ++q)
    {
        R[q][0] = A[q][0] / B[q][0];

        // Quotient rule
        R[q][1] = (A[q][1]*B[q][0] - A[q][0]*B[q][1]) / pow(B[q][0], 2);
        R[q][2] = (A[q][2]*B[q][0] - A[q][0]*B[q][2]) / pow(B[q][0], 2);
        R[q][3] = (A[q][3]*B[q][0] - A[q][0]*B[q][3]) / pow(B[q][0], 2);
    }
    return R;
}

float* min_g(const float (*A)[4], const float (*B)[4],
             float (*R)[4], int c)
{
    for (int q = 0; q < c; ++q)
    {
        if (A[q][0] < B[q][0])
            for (int i=1; i < 4; ++i)
                R[q][i] = A[q][i];
        else
            for (int i=1; i < 4; ++i)
                R[q][i] = B[q][i];
    }
    return R;
}

float* max_g(const float (*A)[4], const float (*B)[4],
             float (*R)[4], int c)
{
    for (int q = 0; q < c; ++q)
    {
        if (A[q][0] >= B[q][0])
            for (int i=1; i < 4; ++i)
                R[q][i] = A[q][i];
        else
            for (int i=1; i < 4; ++i)
                R[q][i] = B[q][i];
    }
    return R;
}

float* pow_g(const float (*A)[4], const float (*B)[4],
             float (*R)[4], int c)
{
    for (int q = 0; q < c; ++q)
    {
        R[q][0] = pow(A[q][0], B[q][0]);

        for (int i=1; i < 4; ++i)
            R[q][i] = pow(A[q][0], B[q][0] - 1) *
                      (A[q][i]*B[q][0] + A[q][0]*B[q][i]*log(A[q][0]))
    }

    return R;
}

////////////////////////////////////////////////////////////////////////////////

float* abs_g(const float (*A)[4], float (*R)[4], int c)
{
    for (int q=0; q < c; ++q)
    {
        R[q][0] = fabs(A[q][0]);
        if (A[q][0] < 0)
            for (int i=1; i < 4; ++i)
                R[q][i] = -A[q][i];
        else
            for (int i=1; i < 4; ++i)
                R[q][i] = A[q][i];
    }

    return R;
}

float* square_g(const float (*A)[4], float (*R)[4], int c)
{
    for (int q = 0; q < c; ++q)
    {
        R[q][0] = A[q][0]*A[q][0];

        // Prouduct rule
        for (int i=1; i < 4; ++i)
            R[q][i] = 2 * A[q][0] * A[q][i];
    }
    return R;
}

float* sqrt_g(const float (*A)[4], float (*R)[4], int c)
{
    for (int q = 0; q < c; ++q)
        if (A[q] < 0)
        {
            for (int i=0; i < 4; ++i)
                R[q][i] = 0;
        }
        else
        {
            R[q][0] = sqrt(A[q][0]);
            for (int i=1; i < 4; ++i)
                R[q][i] = A[q][i] / (2 * sqrt(A[q][0]))
        }
    return R;
}

float* sin_g(const float (*A)[4], float (*R)[4], int c)
{
    for (int q = 0; q < c; ++q)
    {
        R[q][0] = sin(A[q][0]);
        for (int i=1; i < 4; ++i)
            R[q][i] = A[q][i] * cos(A[q][0]);
    }
    return R;
}

float* cos_g(const float (*A)[4], float (*R)[4], int c)
{
    for (int q = 0; q < c; ++q)
    {
        R[q][0] = cos(A[q][0]);
        for (int i=1; i < 4; ++i)
            R[q][i] = A[q][i] * (-sin(A[q][0]));
    }
    return R;
}

float* tan_g(const float (*A)[4], float (*R)[4], int c)
{
    for (int q = 0; q < c; ++q)
    {
        R[q][0] = tan(A[q][0]);
        for (int i=1; i < 4; ++i)
            R[q][i] = A[q][i] / pow(cos(A[q][0]), 2);
    }
    return R;
}

float* asin_g(const float (*A)[4], float (*R)[4], int c)
{
    for (int q = 0; q < c; ++q)
    {
        if (A[q][0] < -1 || A[q][0] > 1)
        {
            for (int i=0; i < 4; ++i)
                R[q][i] = 0;
        }
        else
        {
            R[q][0] = asin(A[q][0]);
            for (int i=1; i < 4; ++i)
                R[q][i] = A[q][i] / sqrt(1 - pow(A[q][0], 2));
        }
    }
    return R;
}

float* acos_g(const float (*A)[4], float (*R)[4], int c)
{
    for (int q = 0; q < c; ++q)
    {
        if (A[q][0] < -1 || A[q][0] > 1)
        {
            for (int i=0; i < 4; ++i)
                R[q][i] = 0;
        }
        else
        {
            R[q][0] = acos(A[q][0]);
            for (int i=1; i < 4; ++i)
                R[q][i] = -A[q][i] / sqrt(1 - pow(A[q][0], 2));
        }
    }
    return R;
}

float* atan_g(const float (*A)[4], float (*R)[4], int c)
{
    for (int q = 0; q < c; ++q)
    {
        R[q][0] = atan(A[q][0]);
        for (int i=1; i < 4; ++i)
            R[q][i] = A[q][i] / (pow(A[q][0], 2) + 1);
    }
    return R;
}

float* neg_g(const float (*A)[4], float (*R)[4], int c)
{
    for (int q=0; q < c; ++q)
        for (int i=0; i < 4; ++i)
            R[q][i] = -A[q][i];
    return R;
}

float* exp_g(const float (*A)[4], float (*R)[4], int c)
{
    for (int q = 0; q < c; ++q)
    {
        R[q][0] = exp(A[q][0]);
        for (int i=1; i < 3; ++i)
            R[q][i] = exp(A[q][0]) * A[q][i];
    }
    return R;
}

////////////////////////////////////////////////////////////////////////////////

float* X_g(const float (*X), float (*R)[4], int c)
{
    for (int q = 0; q < c; ++q)
    {
        R[q][0] = X[q];
        R[q][1] = 1;
        R[q][2] = 0;
        R[q][3] = 0;
    }
    return R;
}

float* Y_g(const float (*Y), float (*R)[4], int c)
{
    for (int q = 0; q < c; ++q)
    {
        R[q][0] = Y[q];
        R[q][1] = 0;
        R[q][2] = 1;
        R[q][3] = 0;
    }
    return R;
}

float* Z_g(const float (*Z), float (*R)[4], int c)
{
    for (int q = 0; q < c; ++q)
    {
        R[q][0] = Z[q];
        R[q][1] = 0;
        R[q][2] = 0;
        R[q][3] = 1;
    }
    return R;
}
