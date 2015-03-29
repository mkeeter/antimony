#ifndef TRIANGULATE_H
#define TRIANGULATE_H

#include <vector>
#include <list>
#include <map>
#include <array>

#include "util/region.h"

void triangulate(struct MathTree_* tree, Region r, bool detect_edges,
                 float** const verts, unsigned* const count);

#endif
