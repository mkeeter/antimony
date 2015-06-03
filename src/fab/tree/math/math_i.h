#ifndef MATH_I_H
#define MATH_I_H

#include "tree/math/math_defines.h"

#include "util/interval.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @file tree/math/math_i.h
    @brief Functions for doing math on intervals
    @details These functions take in input Intervals A and B
    and return the resulting Interval.
*/

// Binary functions
Interval add_i(Interval A, Interval B);
Interval sub_i(Interval A, Interval B);
Interval mul_i(Interval A, Interval B);
Interval div_i(Interval A, Interval B);

Interval min_i(Interval A, Interval B);
Interval max_i(Interval A, Interval B);

Interval pow_i(Interval A, Interval B);

// Unary functions
Interval abs_i(Interval A);
Interval square_i(Interval A);
Interval sqrt_i(Interval A);
Interval sin_i(Interval A);
Interval cos_i(Interval A);
Interval tan_i(Interval A);
Interval asin_i(Interval A);
Interval acos_i(Interval A);
Interval atan_i(Interval A);
Interval neg_i(Interval A);
Interval exp_i(Interval A);

// Variables
Interval X_i(Interval X);
Interval Y_i(Interval Y);
Interval Z_i(Interval Z);

#ifdef __cplusplus
}
#endif

#endif
