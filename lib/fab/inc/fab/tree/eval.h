#ifndef EVAL_H
#define EVAL_H

#include "util/interval.h"
#include "util/region.h"
#include "util/switches.h"

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
struct MathTree_;
struct derivative_;


/** @brief Evaluates a math expression at a given floating-point position.
    @details Results are stored in n->head->results.f
*/
float  eval_f(struct MathTree_* n, const float x, const float y, const float z);


/** @brief Evaluates a math expression over an interval region
    @details Results are stored in n->head->results.i
*/
Interval  eval_i(struct MathTree_* n, const Interval X,
                                        const Interval Y,
                                        const Interval Z);

/** @brief Evaluates a math expression over a set of many positions
    @details Results are stored in n->head->results.r
*/
float*  eval_r(struct MathTree_* n, const Region r);

/** @brief Evaluates partial derivatives over a set of many position.
*/
struct derivative_* eval_g(struct MathTree_* tree, const Region r);

#ifdef __cplusplus
}
#endif

#endif
