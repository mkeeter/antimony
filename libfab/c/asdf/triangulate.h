#ifndef GL_H
#define GL_H

#include <stdint.h>

#include "util/region.h"
#include "util/vec3f.h"

struct ASDF_;
struct Edge_;
struct Mesh_;


/** @brief Recursively triangulates an ASDF
    @details Vertices (and vertex normals) are stored in the array vdata as six consecutive values (3 values each for position and normal)

    vcount keeps track of how many floats have been place in the array
    (so it should always be a factor of 6).

    Triangles are stored as sets of three array indices, with icount recording
    the number of indices saved.  Note that you have to multiply an index by 6 to gets its actual position in the array.
*/
struct Mesh_* triangulate(struct ASDF_* const asdf, volatile int* const halt);

#endif
