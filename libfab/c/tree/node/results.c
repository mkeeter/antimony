#include <stdlib.h>
#include <math.h>

#include "tree/node/node.h"

void fill_results(Node* n, float value)
{
    n->results.f = value;
    n->results.i = (Interval) { .lower=value, .upper=value};

    // Fill the region cache
    for (int q = 0; q < MIN_VOLUME; ++q)
        n->results.r[q] = value;
}
