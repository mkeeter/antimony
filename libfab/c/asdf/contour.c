#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "asdf/asdf.h"
#include "asdf/contour.h"
#include "asdf/neighbors.h"

#include "util/path.h"
#include "util/constants.h"
#include "util/squares.h"

////////////////////////////////////////////////////////////////////////////////
// Forward declarations

/*  write_edges
 *
 *  Writes out paths to the array 'edges', freeing Path nodes as we go.
 */
_STATIC_
void write_edges(const ASDF* const asdf, int* allocated,
                        int* path_count, Path*** const edges);


/*  contour_zero_crossing
 *
 *  Finds a zero crossing on a given edge, returning a Path
 *  pointer with appropriate x and y values.  The path pointer
 *  is also saved in the leaf cell's data array, and ptr[0] or
 *  ptr[1] is set to the address in that array (to allow the path
 *  to remove itself from the leaf later on).
 *
 *  Neighbors should be in the order -y, +y, -x, +x
 *
 */
_STATIC_
Path* contour_zero_crossing(
    ASDF* const leaf, const ASDF* const neighbors[4],
    const uint8_t edge
);


/*  evaluate_pixel
 *
 *  Figures out the behavior of this leaf cell.
 */
_STATIC_
void evaluate_pixel(
    ASDF* const leaf, const ASDF* const neighbors[6]
);


/*  find_edges
 *
 *  Stores edge information in the tree.
 */
_STATIC_
void find_edges(
    ASDF* const asdf, const ASDF* const neighbors[6],
    volatile int* const halt
);

// End of forward declarations
////////////////////////////////////////////////////////////////////////////////

int contour(
    ASDF* const asdf,
    Path*** const paths, volatile int* const halt)
{
    const ASDF* neighbors[4] = {NULL, NULL, NULL, NULL};
    find_edges(asdf, neighbors, halt);

    *paths = malloc(sizeof(Path*));
    int allocated = 1;
    int count = 0;

    write_edges(asdf, &allocated, &count, paths);
    free_data(asdf);

    return count;
}

_STATIC_
void write_edges(
    const ASDF* const asdf, int* allocated,
    int* path_count, Path*** const paths)
{
    if (!asdf)  return;

    if (asdf->state == LEAF) {
        for (int e=0; e < 4; ++e) {
            Path* p = asdf->data.contour[e];
            if (!p) continue;

            // Grab the path and trace it out
            Path* start = backtrace_path(p->prev, p);
            start = decimate_path(start, EPSILON);

            // If we need to allocate more space, then do so.
            if (*path_count >= *allocated) {
                (*allocated) *= 2;
                *paths = realloc(*paths, (*allocated)*sizeof(Path*));
            }
            (*paths)[(*path_count)++] = start;
            disconnect_path(start);
        }
    } else if (asdf->state == BRANCH) {
        // Otherwise, recurse.
        for (int i=0; i < 8; i += 2) {
            write_edges(asdf->branches[i], allocated, path_count, paths);
        }
    }
}


_STATIC_
void find_edges(
    ASDF* const asdf, const ASDF* const neighbors[4],volatile int* const halt)
{
    if (*halt || !asdf) return;

    if (asdf->state == LEAF) {
        evaluate_pixel(asdf, neighbors);
    } else if (asdf->state == BRANCH) {

        // Evaluate the branches
        for (int i=0; i < 8; i+=2) {
            const ASDF* new_neighbors[4];
            get_neighbors_2d(asdf, neighbors, new_neighbors, i);
            find_edges(asdf->branches[i], new_neighbors, halt);
        }


        // Upgrade disconnected edges from children
        // (necessary for multi-scale path merging; trust me on this)
        asdf->data.contour = calloc(4, sizeof(Path*));

        for (int e=0; e < 4; ++e) {

            for (int i=0; i < 8; i += 2) {

                // Only pull from children that touch this edge.
                if (e == 0 &&  (i & 2)) continue;
                if (e == 1 && !(i & 2)) continue;
                if (e == 2 &&  (i & 4)) continue;
                if (e == 3 && !(i & 4)) continue;

                // If we've already filled this edge or
                // this cell doesn't exist or doesn't have data,
                // or doesn't have data on this edge, then skip it.
                if (asdf->data.contour[e] ||
                    !asdf->branches[i] ||
                    !asdf->branches[i]->data.contour ||
                    !asdf->branches[i]->data.contour[e])
                {
                    continue;
                }

                // If this edge has a loose end, upgrade it
                Path* p = asdf->branches[i]->data.contour[e];

                asdf->data.contour[e] = p;

                // Add a pointer so that this path can disconnect
                // itself from the grid when needed
                p->ptrs = realloc(
                    p->ptrs, sizeof(Path**)*(++p->ptr_count)
                );
                p->ptrs[p->ptr_count-1] = &(asdf->data.contour[e]);

            }
        }
    }
}


_STATIC_
void evaluate_pixel(
    ASDF* const leaf, const ASDF* const neighbors[4])
{
    if (!leaf->data.contour) {
        leaf->data.contour = calloc(4, sizeof(Path*));
    }

    // Figure out which of the 16 possible cases we've encountered.
    // (ignore the z levels, just check xy)
    uint8_t lookup = 0;
    for (int vertex=3; vertex >= 0; --vertex) {
        lookup = (lookup << 1) + (leaf->d[vertex << 1] < 0);
    }

    if (EDGE_MAP[lookup][0][0] != -1) {
        Path *p1 = contour_zero_crossing(leaf, neighbors,
                                         EDGE_MAP[lookup][0][0]),
             *p2 = contour_zero_crossing(leaf, neighbors,
                                         EDGE_MAP[lookup][0][1]);
        p1->next = p2;
        p2->prev = p1;
    }

    if (EDGE_MAP[lookup][1][0] != -1) {
        Path *p1 = contour_zero_crossing(leaf, neighbors,
                                         EDGE_MAP[lookup][1][0]),
             *p2 = contour_zero_crossing(leaf, neighbors,
                                         EDGE_MAP[lookup][1][1]);
        p1->next = p2;
        p2->prev = p1;
    }
}



_STATIC_
Path* contour_zero_crossing(
    ASDF* const leaf, const ASDF* const neighbors[4],
    const uint8_t edge)
{
    Path* t = NULL;

    // Check for edge intersections among neighbors:
    // Neighbors are in the order -y, +y, -x, +x

    // Lower edge
    if (edge == 0 && neighbors[0] && neighbors[0]->data.contour) {
        t = neighbors[0]->data.contour[1];
    // Upper edge
    } else if (edge == 1 && neighbors[1] && neighbors[1]->data.contour) {
        t = neighbors[1]->data.contour[0];
    // Left edge
    } else if (edge == 2 && neighbors[2] && neighbors[2]->data.contour) {
        t = neighbors[2]->data.contour[3];
    // Right edge
    } else if (edge == 3 && neighbors[3] && neighbors[3]->data.contour) {
        t = neighbors[3]->data.contour[2];
    }

    // If we've found one, then we're done.
    if (t) {
        leaf->data.contour[edge] = t;
        t->ptrs = realloc(t->ptrs, sizeof(Path**)*(++t->ptr_count));
        t->ptrs[t->ptr_count-1] = &(leaf->data.contour[edge]);
        return t;
    }


    // If we didn't find a match among neighbors, solve for the
    // zero crossing via interpolation.
    const uint8_t v0 = VERTEX_MAP[edge][0]<<1,
                  v1 = VERTEX_MAP[edge][1]<<1;
    const float d0 = leaf->d[v0],
                d1 = leaf->d[v1];

    // Interpolation from d0 to d1
    const float d = (d0)/(d0-d1);

    // Find interpolated coordinates
    const float x0 = (v0 & 4) ? leaf->X.upper : leaf->X.lower,
                y0 = (v0 & 2) ? leaf->Y.upper : leaf->Y.lower;

    const float x1 = (v1 & 4) ? leaf->X.upper : leaf->X.lower,
                y1 = (v1 & 2) ? leaf->Y.upper : leaf->Y.lower;

    Vec3f c = (Vec3f){.x = x0*(1-d)+x1*d,
                      .y = y0*(1-d)+y1*d};

    // Create a point
    t = malloc(sizeof(Path));
    *t = (Path){
        .prev=NULL, .next=NULL,
        .x=c.x, .y=c.y, .z=0,
        .ptrs=malloc(sizeof(Path**)), .ptr_count=1
    };
    leaf->data.contour[edge] = t;

    // Add a back-reference to the containing ASDF's edge pointer
    // (so that the path can disconnect itself when needed)
    t->ptrs[0] = &(leaf->data.contour[edge]);

    return t;
}
