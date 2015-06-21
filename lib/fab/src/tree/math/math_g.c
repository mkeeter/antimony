#include <string.h>
#include <math.h>

#include "fab/tree/math/math_g.h"

derivative* add_g(const derivative* restrict A,
                  const derivative* restrict B,
                  derivative* restrict R, int c)
{
    for (int q = 0; q < c; ++q)
    {
        R[q].v = A[q].v + B[q].v;
        R[q].dx = A[q].dx + B[q].dx;
        R[q].dy = A[q].dy + B[q].dy;
        R[q].dz = A[q].dz + B[q].dz;
    }
    return R;
}

derivative* sub_g(const derivative* restrict A,
                  const derivative* restrict B,
                  derivative* restrict R, int c)
{
    for (int q = 0; q < c; ++q)
    {
        R[q].v = A[q].v - B[q].v;
        R[q].dx = A[q].dx - B[q].dx;
        R[q].dy = A[q].dy - B[q].dy;
        R[q].dz = A[q].dz - B[q].dz;
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
        R[q].dx = A[q].v*B[q].dx + B[q].v*A[q].dx;
        R[q].dy = A[q].v*B[q].dy + B[q].v*A[q].dy;
        R[q].dz = A[q].v*B[q].dz + B[q].v*A[q].dz;
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
        R[q].dx = (B[q].v*A[q].dx - A[q].v*B[q].dx) / p;
        R[q].dy = (B[q].v*A[q].dy - A[q].v*B[q].dy) / p;
        R[q].dz = (B[q].v*A[q].dz - A[q].v*B[q].dz) / p;
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
        {
            R[q].dx = A[q].dx;
            R[q].dy = A[q].dy;
            R[q].dz = A[q].dz;
        }
        else
        {
            R[q].dx = B[q].dx;
            R[q].dy = B[q].dy;
            R[q].dz = B[q].dz;
        }
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
        {
            R[q].dx = A[q].dx;
            R[q].dy = A[q].dy;
            R[q].dz = A[q].dz;
        }
        else
        {
            R[q].dx = B[q].dx;
            R[q].dy = B[q].dy;
            R[q].dz = B[q].dz;
        }
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
        R[q].dx = p * (B[q].v*A[q].dx + (B[q].dx ? m*B[q].dx : 0));
        R[q].dy = p * (B[q].v*A[q].dy + (B[q].dy ? m*B[q].dy : 0));
        R[q].dz = p * (B[q].v*A[q].dz + (B[q].dz ? m*B[q].dz : 0));
    }

    return R;
}

derivative* atan2_g(const derivative* A, const derivative* B, derivative* R, int c) {
    for (int q = 0; q < c; ++q)
    {
        R[q].v = atan2(A[q].v, B[q].v);
        const float d = pow(A[q].v, 2) + pow(B[q].v, 2);
        R[q].dx = (-A[q].dx*B[q].v + B[q].dx*A[q].v) / d;
        R[q].dy = (-A[q].dy*B[q].v + B[q].dy*A[q].v) / d;
        R[q].dz = (-A[q].dz*B[q].v + B[q].dz*A[q].v) / d;
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
        {
            R[q].dx = -A[q].dx;
            R[q].dy = -A[q].dy;
            R[q].dz = -A[q].dz;
        }
        else
        {
            R[q].dx = A[q].dx;
            R[q].dy = A[q].dy;
            R[q].dz = A[q].dz;
        }
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
        R[q].dx = a * A[q].dx;
        R[q].dy = a * A[q].dy;
        R[q].dz = a * A[q].dz;
    }
    return R;
}

derivative* sqrt_g(const derivative* restrict A,
                   derivative* restrict R, int c)
{
    for (int q = 0; q < c; ++q)
        if (A[q].v < 0)
        {
            R[q].v = 0;
            R[q].dx = 0;
            R[q].dy = 0;
            R[q].dz = 0;
        }
        else
        {
            R[q].v = sqrt(A[q].v);
            const float r = R[q].v * 2;
            R[q].dx = A[q].dx / r;
            R[q].dy = A[q].dy / r;
            R[q].dz = A[q].dz / r;
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
        R[q].dx = A[q].dx * c;
        R[q].dy = A[q].dy * c;
        R[q].dz = A[q].dz * c;
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
        R[q].dx = A[q].dx * s;
        R[q].dy = A[q].dy * s;
        R[q].dz = A[q].dz * s;
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
        R[q].dx = A[q].dx / d;
        R[q].dy = A[q].dy / d;
        R[q].dz = A[q].dz / d;
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
            R[q].v = 0;
            R[q].dx = 0;
            R[q].dy = 0;
            R[q].dz = 0;
        }
        else
        {
            R[q].v = asin(A[q].v);
            const float d = sqrt(1 - pow(A[q].v, 2));
            R[q].dx = A[q].dx / d;
            R[q].dy = A[q].dy / d;
            R[q].dz = A[q].dz / d;
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
            R[q].v = 0;
            R[q].dx = 0;
            R[q].dy = 0;
            R[q].dz = 0;
        }
        else
        {
            R[q].v = acos(A[q].v);
            const float d = -sqrt(1 - pow(A[q].v, 2));
            R[q].dx = A[q].dx / d;
            R[q].dy = A[q].dy / d;
            R[q].dz = A[q].dz / d;
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
        R[q].dx = A[q].dx / d;
        R[q].dy = A[q].dy / d;
        R[q].dz = A[q].dz / d;
    }
    return R;
}

derivative* neg_g(const derivative* restrict A,
                   derivative* restrict R, int c)
{
    for (int q=0; q < c; ++q)
    {
        R[q].v  = -A[q].v;
        R[q].dx = -A[q].dx;
        R[q].dy = -A[q].dy;
        R[q].dz = -A[q].dz;
    }
    return R;
}

derivative* exp_g(const derivative* restrict A,
                   derivative* restrict R, int c)
{
    for (int q = 0; q < c; ++q)
    {
        R[q].v = exp(A[q].v);
        const float e = exp(A[q].v);
        R[q].dx = A[q].dx * e;
        R[q].dy = A[q].dy * e;
        R[q].dz = A[q].dz * e;
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
