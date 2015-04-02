#include <algorithm>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "triangulate.h"
#include "triangulate/mesher.h"

#include "tree/tree.h"

// Finds an array of vertices (as x,y,z float triplets).
// Sets *count to the number of vertices returned.
void triangulate(MathTree* tree, const Region r,
                 bool detect_edges, volatile int* halt,
                 float** const verts, unsigned* const count)
{
    Mesher t(tree, detect_edges, halt);

    // Top-level call to the recursive triangulation function.
    t.triangulate_region(r);

    // Copy data from tristate struct to output pointers.
    *verts = t.get_verts(count);
}
