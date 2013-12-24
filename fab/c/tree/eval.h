#ifndef EVAL_H
#define EVAL_H

#include "util/interval.h"
#include "util/region.h"
#include "util/switches.h"

// Forward declarations
struct PackedTree_;


/** @brief Evaluates a math expression at a given floating-point position.
    @details Results are stored in n->head->results.f
*/
float  eval_f(struct PackedTree_* n, const float x, const float y, const float z);


/** @brief Evaluates a math expression over an interval region
    @details Results are stored in n->head->results.i
*/
Interval  eval_i(struct PackedTree_* n, const Interval X,
                                        const Interval Y,
                                        const Interval Z);

/** @brief Evaluates a math expression over a set of many positions
    @details Results are stored in n->head->results.r
*/
float*  eval_r(struct PackedTree_* n, const Region r);

#endif
