#include <string.h>
#include <math.h>

#include "fab/tree/math/math_g.h"

// Cast the struct to an array so that we can iterate over it.
#define INDEX(A) ((float*)(&A[q]))[i]

derivative* add_g(const derivative* restrict A,
                  const derivative* restrict B,
                  derivative* restrict R, int c)
{
    for (int q = 0; q < c; ++q)
        for (int i=0; i < 4; ++i)
            INDEX(R) = INDEX(A) + INDEX(B);
    return R;
}

derivative* sub_g(const derivative* restrict A,
                  const derivative* restrict B,
                  derivative* restrict R, int c)
{
    for (int q = 0; q < c; ++q)
    {
        for (int i=0; i < 4; ++i)
            INDEX(R) = INDEX(A) - INDEX(B);
    }
    return R;
}

derivative* mul_g(const derivative* restrict A,
                  const derivative* restrict B,
                  derivative* restrict R, int c)
{
    for (int q = 0; q < c; ++q)
    {
        R[q].v = A[q].v * B[q].v;

        // Product rule
        for (int i=1; i < 4; ++i)
            INDEX(R) = A[q].v*INDEX(B) + INDEX(A)*B[q].v;
    }
    return R;
}

derivative* div_g(const derivative* restrict A,
                  const derivative* restrict B,
                  derivative* restrict R, int c)
{
    for (int q = 0; q < c; ++q)
    {
        R[q].v = A[q].v / B[q].v;

        // Quotient rule
        const float p = pow(B[q].v, 2);
        for (int i=1; i < 4; ++i)
            INDEX(R) = (INDEX(A)*B[q].v - A[q].v*INDEX(B)) / p;
    }
    return R;
}

derivative* min_g(const derivative* restrict A,
                  const derivative* restrict B,
                  derivative* restrict R, int c)
{
    for (int q = 0; q < c; ++q)
    {
        R[q].v = fmin(A[q].v, B[q].v);
        if (A[q].v < B[q].v)
            for (int i=1; i < 4; ++i)
                INDEX(R) = INDEX(A);
        else
            for (int i=1; i < 4; ++i)
                INDEX(R) = INDEX(B);
    }
    return R;
}

derivative* max_g(const derivative* restrict A,
                  const derivative* restrict B,
                  derivative* restrict R, int c)
{
    for (int q = 0; q < c; ++q)
    {
        R[q].v = fmax(A[q].v, B[q].v);
        if (A[q].v >= B[q].v)
            for (int i=1; i < 4; ++i)
                INDEX(R) = INDEX(A);
        else
            for (int i=1; i < 4; ++i)
                INDEX(R) = INDEX(B);
    }
    return R;
}

derivative* pow_g(const derivative* restrict A,
                  const derivative* restrict B,
                  derivative* restrict R, int c)
{
    for (int q = 0; q < c; ++q)
    {
        R[q].v = pow(A[q].v, B[q].v);

        const float p = pow(A[q].v, B[q].v - 1);
        const float m = A[q].v * log(A[q].v);
        // If A[q].v is negative, then m will be NaN (because of log's domain).
        // We work around this by checking if d/d{xyz}(B) == 0 and using a
        // simplified expression if that's true.
        for (int i=1; i < 4; ++i)
            INDEX(R) = INDEX(B) ? p * (INDEX(A)*B[q].v + m * INDEX(B))
                                : p * (INDEX(A)*B[q].v);
    }

    return R;
}

////////////////////////////////////////////////////////////////////////////////

derivative* abs_g(const derivative* restrict A,
                  derivative* restrict R, int c)
{
    for (int q=0; q < c; ++q)
    {
        R[q].v = fabs(A[q].v);
        if (A[q].v < 0)
            for (int i=1; i < 4; ++i)
                INDEX(R) = -INDEX(A);
        else
            for (int i=1; i < 4; ++i)
                INDEX(R) = INDEX(A);
    }

    return R;
}

derivative* square_g(const derivative* restrict A,
                     derivative* restrict R, int c)
{
    for (int q = 0; q < c; ++q)
    {
        R[q].v = A[q].v*A[q].v;

        // Prouduct rule
        const float a = 2 * A[q].v;
        for (int i=1; i < 4; ++i)
            INDEX(R) = a * INDEX(A);
    }
    return R;
}

derivative* sqrt_g(const derivative* restrict A,
                   derivative* restrict R, int c)
{
    for (int q = 0; q < c; ++q)
        if (A[q].v < 0)
        {
            for (int i=0; i < 4; ++i)
                INDEX(R) = 0;
        }
        else
        {
            R[q].v = sqrt(A[q].v);
            const float r = R[q].v * 2;
            for (int i=1; i < 4; ++i)
                INDEX(R) = INDEX(A) / r;
        }
    return R;
}

derivative* sin_g(const derivative* restrict A,
                  derivative* restrict R, int c)
{
    for (int q = 0; q < c; ++q)
    {
        R[q].v = sin(A[q].v);
        const float c = cos(A[q].v);
        for (int i=1; i < 4; ++i)
            INDEX(R) = INDEX(A) * c;
    }
    return R;
}

derivative* cos_g(const derivative* restrict A,
                  derivative* restrict R, int c)
{
    for (int q = 0; q < c; ++q)
    {
        R[q].v = cos(A[q].v);
        const float s = -sin(A[q].v);
        for (int i=1; i < 4; ++i)
            INDEX(R) = INDEX(A) * s;
    }
    return R;
}

derivative* tan_g(const derivative* restrict A,
                  derivative* restrict R, int c)
{
    for (int q = 0; q < c; ++q)
    {
        R[q].v = tan(A[q].v);
        const float d = pow(cos(A[q].v), 2);
        for (int i=1; i < 4; ++i)
            INDEX(R) = INDEX(A) / d;
    }
    return R;
}

derivative* asin_g(const derivative* restrict A,
                   derivative* restrict R, int c)
{
    for (int q = 0; q < c; ++q)
    {
        if (A[q].v < -1 || A[q].v > 1)
        {
            for (int i=0; i < 4; ++i)
                INDEX(R) = 0;
        }
        else
        {
            R[q].v = asin(A[q].v);
            const float d = sqrt(1 - pow(A[q].v, 2));
            for (int i=1; i < 4; ++i)
                INDEX(R) = INDEX(A) / d;
        }
    }
    return R;
}

derivative* acos_g(const derivative* restrict A,
                   derivative* restrict R, int c)
{
    for (int q = 0; q < c; ++q)
    {
        if (A[q].v < -1 || A[q].v > 1)
        {
            for (int i=0; i < 4; ++i)
                INDEX(R) = 0;
        }
        else
        {
            R[q].v = acos(A[q].v);
            const float d = -sqrt(1 - pow(A[q].v, 2));
            for (int i=1; i < 4; ++i)
                INDEX(R) = INDEX(A) / d;
        }
    }
    return R;
}

derivative* atan_g(const derivative* restrict A,
                   derivative* restrict R, int c)
{
    for (int q = 0; q < c; ++q)
    {
        R[q].v = atan(A[q].v);
        const float d = pow(A[q].v, 2) + 1;
        for (int i=1; i < 4; ++i)
            INDEX(R) = INDEX(A) / d;
    }
    return R;
}

derivative* neg_g(const derivative* restrict A,
                   derivative* restrict R, int c)
{
    for (int q=0; q < c; ++q)
        for (int i=0; i < 4; ++i)
            INDEX(R) = -INDEX(A);
    return R;
}

derivative* exp_g(const derivative* restrict A,
                   derivative* restrict R, int c)
{
    for (int q = 0; q < c; ++q)
    {
        R[q].v = exp(A[q].v);
        const float e = exp(A[q].v);
        for (int i=1; i < 4; ++i)
            INDEX(R) = e * INDEX(A);
    }
    return R;
}

////////////////////////////////////////////////////////////////////////////////

derivative* X_g(const float* restrict X,
                derivative* restrict R, int c)
{
    for (int q = 0; q < c; ++q)
    {
        R[q].v = X[q];
        R[q].dx = 1;
        R[q].dy = 0;
        R[q].dz = 0;
    }
    return R;
}

derivative* Y_g(const float* restrict Y,
                derivative* restrict R, int c)
{
    for (int q = 0; q < c; ++q)
    {
        R[q].v = Y[q];
        R[q].dx = 0;
        R[q].dy = 1;
        R[q].dz = 0;
    }
    return R;
}

derivative* Z_g(const float* restrict Z,
                derivative* restrict R, int c)
{
    for (int q = 0; q < c; ++q)
    {
        R[q].v = Z[q];
        R[q].dx = 0;
        R[q].dy = 0;
        R[q].dz = 1;
    }
    return R;
}
