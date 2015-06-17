#include <stdlib.h>
#include <math.h>

#include "tree/node/node.h"
#include "tree/math/math_g.h"

void fill_results(Node* n, float value)
{
    n->results.f = value;
    n->results.i = (Interval) { .lower=value, .upper=value};

    // Fill the region cache
    for (int q = 0; q < MIN_VOLUME; ++q)
        n->results.r[q] = value;
}

void fill_results_g(Node* n, float value)
{
    for (int q=0; q < MIN_VOLUME/4; ++q)
    {
        ((derivative*)(n->results.r))[q].v = value;
        ((derivative*)(n->results.r))[q].dx = 0;
        ((derivative*)(n->results.r))[q].dy = 0;
        ((derivative*)(n->results.r))[q].dz = 0;
    }
}
