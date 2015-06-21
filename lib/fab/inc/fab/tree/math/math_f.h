#ifndef MATH_F_H
#define MATH_F_H

#include <math.h>

#include "fab/tree/math/math_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @file tree/math/math_f.h
    @brief Functions for doing math on floating-point numbers.
    @details These functions take in input floats A and B,
    and return the result of their computation.
*/

// Binary functions
inline float add_f(float A, float B) { return A+B; }
inline float sub_f(float A, float B) { return A-B; }
inline float mul_f(float A, float B) { return A*B; }
inline float div_f(float A, float B) { return A/B; }
inline float min_f(float A, float B) { return fmin(A, B); }
inline float max_f(float A, float B) { return fmax(A, B); }
inline float pow_f(float A, float B) { return pow(A, B); }

////////////////////////////////////////////////////////////////////////////////

inline float abs_f(float A) { return fabs(A); }
inline float square_f(float A) { return A*A; }
inline float sqrt_f(float A) { return A < 0 ? 0 : sqrt(A); }
inline float sin_f(float A) { return sin(A); }
inline float cos_f(float A) { return cos(A); }
inline float tan_f(float A) { return tan(A); }

inline float asin_f(float A)
{
    if (A < -1)     return -M_PI_2;
    else if (A > 1) return M_PI_2;
    else            return asin(A);
}

inline float acos_f(float A)
{
    if (A < -1)     return M_PI;
    else if (A > 1) return 0;
    else            return acos(A);
}

inline float atan_f(float A) { return atan(A); }
inline float atan2_f(float A, float B) { return atan2(A, B); }
inline float neg_f(float A) { return -A; }
inline float exp_f(float A) { return exp(A); }

////////////////////////////////////////////////////////////////////////////////

inline float X_f(float X) { return X; }
inline float Y_f(float Y) { return Y; }
inline float Z_f(float Z) { return Z; }

#ifdef __cplusplus
}
#endif

#endif
