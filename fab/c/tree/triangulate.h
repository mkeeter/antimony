#ifndef TRIANGULATE_H
#define TRIANGULATE_H

#include "util/region.h"

struct MathTree_;

void triangulate(struct MathTree_* tree, Region r,
                 float** const verts, unsigned* const count);

#endif
