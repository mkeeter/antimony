#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "tree/math/math_i.h"

Interval add_i(Interval A, Interval B)
{
    return (Interval){.lower = A.lower + B.lower,
                      .upper = A.upper + B.upper};
}

Interval sub_i(Interval A, Interval B)
{
    return (Interval){.lower = A.lower - B.upper,
                      .upper = A.upper - B.lower};
}

Interval mul_i(Interval A, Interval B)
{
    Interval i;

    float c1 = A.lower * B.lower,
          c2 = A.lower * B.upper,
          c3 = A.upper * B.lower,
          c4 = A.upper * B.upper;

    i.lower = fmin(fmin(c1, c2), fmin(c3, c4));
    i.upper = fmax(fmax(c1, c2), fmax(c3, c4));
    return i;
}

Interval div_i(Interval A, Interval B)
{
    Interval i;

    if (B.lower <=0 && B.upper >= 0)
    {
        i.lower = -INFINITY;
        i.upper = INFINITY;
        return i;
    }

    B.lower = 1/B.lower;
    B.upper = 1/B.upper;
    float c1 = A.lower * B.lower,
          c2 = A.lower * B.upper,
          c3 = A.upper * B.lower,
          c4 = A.upper * B.upper;

    i.lower = fmin(fmin(c1, c2), fmin(c3, c4));
    i.upper = fmax(fmax(c1, c2), fmax(c3, c4));
    return i;
}

Interval min_i(Interval A, Interval B)
{
    Interval i;

    i.lower = A.lower < B.lower ? A.lower : B.lower;
    i.upper = A.upper < B.upper ? A.upper : B.upper;
    return i;
}

Interval max_i(Interval A, Interval B)
{
    Interval i;

    i.lower = A.lower > B.lower ? A.lower : B.lower;
    i.upper = A.upper > B.upper ? A.upper : B.upper;
    return i;
}

Interval pow_i(Interval A, Interval B)
{
    Interval i;

    int p = B.lower;

    if (p % 2) {
        i.lower = pow(A.lower, p);
        i.upper = pow(A.upper, p);
    } else {
        float L = fabs(A.lower), U = fabs(A.upper);
        if (A.lower <= 0 && A.upper >= 0)
            i.lower = 0;
        else
            i.lower = pow(L < U ? L : U, p);
        i.upper = pow(L < U ? U : L, p);
    }

    return i;
}

////////////////////////////////////////////////////////////////////////////////

Interval abs_i(Interval A)
{
    Interval i;

    if (A.lower < 0)
        i.lower = 0;
    else
        i.lower = fmin(fabs(A.lower), fabs(A.upper));
    i.upper = fmax(fabs(A.lower), fabs(A.upper));

    return i;
}

Interval square_i(Interval A)
{
    Interval i;

    float u = A.upper * A.upper;
    float l = A.lower * A.lower;

    if (A.upper > 0 && A.lower < 0)
        i.lower = 0;
    else
        i.lower = fmin(u, l);
    i.upper = fmax(u, l);

    return i;
}

Interval sqrt_i(Interval A)
{
    Interval i;
    if (A.lower <= 0)   i.lower = 0;
    else                i.lower = sqrt(A.lower);

    if (A.upper <= 0)   i.upper = 0;
    else                i.upper = sqrt(A.upper);

    return i;
}

Interval sin_i(Interval A)
{
    Interval i;

    if (A.lower == A.upper) {
        i.lower = sin(A.lower);
        i.upper = i.lower;
    } else {
        i.lower = -1;
        i.upper = 1;
    }
    return i;
}

Interval cos_i(Interval A)
{
    Interval i;

    if (A.lower == A.upper) {
        i.lower = cos(A.lower);
        i.upper = i.lower;
    } else {
        i.lower = -1;
        i.upper = 1;
    }
    return i;
}

Interval tan_i(Interval A)
{
    Interval i;

    if (A.lower == A.upper) {
        i.lower = tan(A.lower);
        i.upper = i.lower;
    } else {
        i.lower = -INFINITY;
        i.upper =  INFINITY;
    }
    return i;
}

Interval asin_i(Interval A)
{
    Interval i;

    if (A.lower == A.upper) {
        if (A.lower <= -1)      i.lower = -M_PI_2;
        else if (A.lower >= 1)  i.lower = M_PI_2;
        else                    i.lower = asin(A.lower);
        i.upper = i.lower;
    } else {
        if (A.lower <= -1)      i.lower = -M_PI_2;
        else if (A.lower >= 1)  i.lower = M_PI_2;
        else                    i.lower = asin(A.lower);

        if (A.upper <= -1)      i.upper = -M_PI_2;
        else if (A.upper >= 1)  i.upper = M_PI_2;
        else                    i.upper = asin(A.upper);
    }

    return i;
}

Interval acos_i(Interval A)
{
    Interval i;

    if (A.lower == A.upper) {
        if (A.lower <= -1)      i.lower = M_PI;
        else if (A.lower >= 1)  i.lower = 0;
        else                    i.lower = acos(A.lower);
        i.upper = i.lower;
    } else {
        if (A.lower <= -1)      i.upper = M_PI;
        else if (A.lower > 1)   i.upper = 0;
        else                    i.upper = acos(A.lower);

        if (A.upper <= -1)      i.lower = M_PI;
        else if (A.upper > 1)   i.lower = 0;
        else                    i.lower = acos(A.upper);
    }

    return i;
}

Interval atan_i(Interval A)
{
    Interval i;

    if (A.lower == A.upper) {
        i.lower = atan(A.lower);
        i.upper = i.lower;
    } else {
        i.lower = atan(A.lower);
        i.upper = atan(A.upper);
    }

    return i;
}

Interval neg_i(Interval A)
{
    return (Interval){.lower = -A.upper,
                      .upper = -A.lower};
}

Interval exp_i(Interval A)
{
    return (Interval){.lower = exp(A.lower),
                      .upper = exp(A.upper)};
}

////////////////////////////////////////////////////////////////////////////////

Interval X_i(Interval X)
{ return X; }

Interval Y_i(Interval Y)
{ return Y; }

Interval Z_i(Interval Z)
{ return Z; }
