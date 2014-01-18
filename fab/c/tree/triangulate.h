#ifndef TRIANGULATE_H
#define TRIANGULATE_H

#include "util/region.h"

struct MathTree_;

float* triangulate(struct MathTree_* tree, Region r, unsigned* const out);

#endif
