#include <math.h>

#include "tree/math/math_f.h"

float add_f(float A, float B)
{
    return A+B;
}

float sub_f(float A, float B)
{
    return A-B;
}

float mul_f(float A, float B)
{
    return A*B;
}

float div_f(float A, float B)
{
    return A/B;
}

float min_f(float A, float B)
{
    return A < B ? A : B;
}

float max_f(float A, float B)
{
    return A > B ? A : B;
}

float pow_f(float A, float B)
{
    return pow(A, B);
}

////////////////////////////////////////////////////////////////////////////////

float abs_f(float A)
{
    return fabs(A);
}

float square_f(float A)
{
    return A*A;
}

float sqrt_f(float A)
{
    if (A < 0)  return 0;
    else        return sqrt(A);
}

float sin_f(float A)
{
    return sin(A);
}

float cos_f(float A)
{
    return cos(A);
}

float tan_f(float A)
{
    return tan(A);
}

float asin_f(float A)
{
    if (A < -1)     return -M_PI_2;
    else if (A > 1) return M_PI_2;
    else            return asin(A);
}

float acos_f(float A)
{
    if (A < -1)     return M_PI;
    else if (A > 1) return 0;
    else            return acos(A);
}

float atan_f(float A)
{
    return atan(A);
}

float neg_f(float A)
{
    return -A;
}

float exp_f(float A)
{
    return exp(A);
}

////////////////////////////////////////////////////////////////////////////////

float X_f(float X)
{ return X; }

float Y_f(float Y)
{ return Y; }

float Z_f(float Z)
{ return Z; }
