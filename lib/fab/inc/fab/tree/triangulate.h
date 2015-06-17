#ifndef TRIANGULATE_H
#define TRIANGULATE_H

#include <vector>
#include <list>
#include <map>
#include <array>

#include "fab/util/region.h"

void triangulate(struct MathTree_* tree, Region r,
                 bool detect_edges, volatile int* halt,
                 float** const verts, unsigned* const count);

#endif
