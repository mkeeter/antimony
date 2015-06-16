#ifndef CACHE_H
#define CACHE_H

#include "util/interval.h"
#include "util/region.h"
#include "util/switches.h"

#ifdef __cplusplus
extern "C" {
#endif

struct Node_;

/** @struct Results_
    @brief Container for intermediate calculation results
*/
typedef struct Results_
{
    float    f;
    Interval i;
    float    r[MIN_VOLUME];
} Results;


/** @brief Fills node results with a constant
    @details n->results.{f,i,r} are all set equal to the constant
    @param n Target node
    @param value Constant to fill
*/
void fill_results(struct Node_* n, float value);

/*
 *  Fills the results array with appropriate values for partial derivatives.
 */
void fill_results_g(struct Node_* n, float value);

#ifdef __cplusplus
}
#endif

#endif
