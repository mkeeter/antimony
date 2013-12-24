#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "asdf/asdf.h"
#include "asdf/cache.h"

#include "tree/eval.h"
#include "tree/packed.h"
#include "util/interval.h"
#include "util/constants.h"

#include "util/switches.h"


////////////////////////////////////////////////////////////////////////////////
//  Forward declarations of _STATIC_ functions

/** @brief Expands A to include B
    @details Copies over appropriate d values
    @param axis 4, 2, or 1 for x, y, or z axis merge
*/
_STATIC_
void merge_cells(ASDF* const A, ASDF* const B, const int axis);


/** @brief Uses regional evaluation to build a chunk of the ASDF tree
    @details Uses eval_r on the PackedTree to evaluate an array of points
    all at once, then collapses them into an ASDF.
    @param tree Target PackedTree
    @param region Region on which to evaluate
    @param merge_leafs Boolean determining whether leaf cells are merged
*/
_STATIC_
ASDF*  build_asdf_region(struct PackedTree_* tree,
                         Region region, const _Bool merge_leafs);
_STATIC_
ASDF* _build_asdf_region(const float* const result, const Region region,
                         const int kstride, const int jstride,
                         const _Bool merge_leafs);

/** @brief Finds the minimum cell sizes along each dimension.
    @param dx Minimum x size
    @param dy Minimum y size
    @param dz Minimum z size
 */
_STATIC_
void find_min_sizes(const ASDF* const asdf, float* dx, float* dy, float* dz);

////////////////////////////////////////////////////////////////////////////////


// Create a single ASDF node, evaluating the corners without recursion.
ASDF* asdf_root(PackedTree* tree, Region region)
{
    // Allocate an ASDF structure
    ASDF* asdf = calloc(1, sizeof(ASDF));

    // Save the corners of the world
    *asdf = (ASDF){
        .state = BRANCH,
        .X = (Interval){region.X[0], region.X[region.ni]},
        .Y = (Interval){region.Y[0], region.Y[region.nj]},
        .Z = (Interval){region.Z[0], region.Z[region.nk]},
    };

    // Sample the distance field at the corners of the cell
    for (int n=0; n < 8; ++n)
        asdf->d[n] = eval_f(tree,
            (n & 4) ? asdf->X.upper : asdf->X.lower,
            (n & 2) ? asdf->Y.upper : asdf->Y.lower,
            (n & 1) ? asdf->Z.upper : asdf->Z.lower);

    return asdf;
}

////////////////////////////////////////////////////////////////////////////////

ASDF* build_asdf(
    PackedTree* const tree, const Region region, const _Bool merge_leafs, volatile int* const halt)
{
    // Special interrupt system, set asynchronously by on high
    if (*halt) return NULL;

    if ((region.ni+1)*(region.nj+1)*(region.nk+1) < MIN_VOLUME)
        return build_asdf_region(tree, region, merge_leafs);

    // Allocate an ASDF structure
    ASDF* asdf = calloc(1, sizeof(ASDF));

    // Save the corners of the world
    *asdf = (ASDF){
        .state = BRANCH,
        .X = (Interval){region.X[0], region.X[region.ni]},
        .Y = (Interval){region.Y[0], region.Y[region.nj]},
        .Z = (Interval){region.Z[0], region.Z[region.nk]},
    };

    // Decide if we should recurse down the tree
    _Bool recurse = true;
    if (region.voxels == 1) {
        recurse = false;
    } else {
        const Interval result = eval_i(tree, asdf->X, asdf->Y, asdf->Z);
        if (result.lower >= 0 || result.upper < 0) {
            recurse = false;
        }
    }

    // If this is a terminal cell, populate corners with sampled
    // values and set an appropriate state.
    if (!recurse) {
        _Bool empty  = true;
        _Bool filled = true;

        for (int n=0; n < 8; ++n) {
            asdf->d[n] = eval_f(tree,
                (n & 4) ? asdf->X.upper : asdf->X.lower,
                (n & 2) ? asdf->Y.upper : asdf->Y.lower,
                (n & 1) ? asdf->Z.upper : asdf->Z.lower);

            if (asdf->d[n] < 0)     empty = false;
            else                    filled = false;
        }

        if (empty)          asdf->state = EMPTY;
        else if (filled)    asdf->state = FILLED;
        else                asdf->state = LEAF;

    } else {
        #if PRUNE
            disable_nodes(tree);
        #endif

        Region octants[8];
        const uint8_t bits = octsect_active(region, tree, octants);

        // If there's at least one active axis along which we can
        // split the system, then subdivide and recurse
        if (bits > 1) {
            // Fill in up to eight octants, depending on whether we can still
            // split the region along this particular axis
            for (int i=0; i < 8; ++i) {
                if (bits & (1 << i)) {
                    asdf->branches[i] = build_asdf(tree, octants[i],
                                                   merge_leafs, halt);
                }
            }
        }
        // Otherwise, we'll transform this into a leaf cell.
        else {
            for (int n=0; n < 8; ++n) {
                asdf->d[n] = eval_f(tree,
                    (n & 4) ? asdf->X.upper : asdf->X.lower,
                    (n & 2) ? asdf->Y.upper : asdf->Y.lower,
                    (n & 1) ? asdf->Z.upper : asdf->Z.lower);
            }
            asdf->state = LEAF;
        }

        #if PRUNE
            enable_nodes(tree);
        #endif

        if (*halt) {
            free_asdf(asdf);
            return NULL;
        }

        // Pull d values from children
        get_d_from_children(asdf);

        // Merge cells if possible
        simplify(asdf, merge_leafs);
    }


    return asdf;
}

////////////////////////////////////////////////////////////////////////////////

_STATIC_
ASDF* build_asdf_region(PackedTree* tree, Region region,
                        const _Bool merge_leafs)
{
    const int voxels = (region.ni+1)*(region.nj+1)*(region.nk+1);

    float *X = malloc(voxels*sizeof(float)),
          *Y = malloc(voxels*sizeof(float)),
          *Z = malloc(voxels*sizeof(float));

    // Copy the X, Y, Z vectors into a flattened matrix form.
    int q = 0;
    for (int k = 0; k <= region.nk; ++k) {
        for (int j = 0; j <= region.nj; ++j) {
            for (int i = 0; i <= region.ni; ++i) {
                X[q] = region.X[i];
                Y[q] = region.Y[j];
                Z[q] = region.Z[k];
                q++;
            }
        }
    }

    // Create a dummy region with the flattened vectors
    Region r = (Region) {.X = X, .Y = Y, .Z = Z, .voxels = voxels };
    const float* const result = eval_r(tree, r);

    // Free the allocated matrices
    free(X);
    free(Y);
    free(Z);

    // Calculate k and j stride in the results array
    const int kstride = (region.nj+1)*(region.ni+1);
    const int jstride = region.ni+1;

    // Set these values to zero so that lookups will index
    // into the results array at the correct point
    region.imin = 0;
    region.jmin = 0;
    region.kmin = 0;

    return _build_asdf_region(result, region, kstride, jstride, merge_leafs);
}

_STATIC_
ASDF* _build_asdf_region(const float* const result, const Region region,
                         const int kstride, const int jstride,
                         const _Bool merge_leafs)
{
    ASDF* const asdf = calloc(1, sizeof(ASDF));
    *asdf = (ASDF) {
        .state = BRANCH,
        .X = (Interval){region.X[0], region.X[region.ni]},
        .Y = (Interval){region.Y[0], region.Y[region.nj]},
        .Z = (Interval){region.Z[0], region.Z[region.nk]}
    };


    // Scan across samples to see if this cell is filled or empty.
    int count = 0;
    int total = 0;
    for (int k=region.kmin; k <= region.kmin + region.nk; ++k) {
        for (int j=region.jmin; j <= region.jmin + region.nj; ++j) {
            for (int i=region.imin; i <= region.imin + region.ni; ++i) {
                if (result[k*kstride+j*jstride+i] < 0)  count++;
                else                                    count--;
                total++;
            }
        }
    }

    // Populate the corners of the ASDF by looking up values in
    // the array of results.
    for (int n=0; n<8; ++n) {
        int i = region.imin + ((n & 4) ? region.ni : 0),
            j = region.jmin + ((n & 2) ? region.nj : 0),
            k = region.kmin + ((n & 1) ? region.nk : 0);
        asdf->d[n] = result[k*kstride + j*jstride + i];
    }

    if (count == total)             asdf->state = FILLED;
    else if (count == -total)       asdf->state = EMPTY;
    else if (region.voxels == 1)    asdf->state = LEAF;
    else {
        // Split and recurse
        Region octants[8];
        uint8_t bits = octsect(region, octants);
        for (int i=0; i < 8; ++i) {
            if (bits & (1 << i)) {
                asdf->branches[i] = _build_asdf_region(
                    result, octants[i], kstride, jstride, merge_leafs
                );
            }
        }

        // Pull d[i] values from children nodes
        get_d_from_children(asdf);

        // Merge cells if possible
        simplify(asdf, merge_leafs);
    }

    return asdf;
}

////////////////////////////////////////////////////////////////////////////////

ASDF* split_cell(ASDF* const asdf, const ASDF* neighbor, const uint8_t axis)
{
    if (asdf == NULL)   return NULL;

    assert(axis == 4 || axis == 2 || axis == 1);

    while (!neighbor->branches[axis])
    {
        neighbor = neighbor->branches[0];
    }
    float new_pos;
    if (axis == 4)      new_pos = neighbor->branches[0]->X.upper;
    else if (axis == 2) new_pos = neighbor->branches[0]->Y.upper;
    else                new_pos = neighbor->branches[0]->Z.upper;

    ASDF* new = malloc(sizeof(ASDF));
    *new = (ASDF) {
       .state=asdf->state,
       .X=(Interval){asdf->X.lower, asdf->X.upper},
       .Y=(Interval){asdf->Y.lower, asdf->Y.upper},
       .Z=(Interval){asdf->Z.lower, asdf->Z.upper},
       .branches = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
       .data = {.vp = NULL}
    };

    // Adjust the lower bound of the split axis in the new cell
    if (axis == 4)          new->X.lower = new_pos;
    else if (axis == 2)     new->Y.lower = new_pos;
    else if (axis == 1)     new->Z.lower = new_pos;

    // Find new distance samples with interpolation
    for (int i=0; i < 8; ++i) {
        new->d[i] = asdf_interpolate(asdf,
            (i & 4) ? new->X.upper : new->X.lower,
            (i & 2) ? new->Y.upper : new->Y.lower,
            (i & 1) ? new->Z.upper : new->Z.lower
        );
    }

    // Modify the upper bound of the split axis in the orignal cell
    if (axis == 4)          asdf->X.upper = new->X.lower;
    else if (axis == 2)     asdf->Y.upper = new->Y.lower;
    else if (axis == 1)     asdf->Z.upper = new->Z.lower;

    // Copy over the interpolated d values on the boundary
    for (int i=0; i < 8; ++i) {
        if (i & axis) {
            asdf->d[i] = new->d[i & ~axis];
        }
    }

    // Special case for branches: either reorganize the branches or
    // recursively split child cells.
    if (asdf->state == BRANCH) {
        for (int i=0; i < 8; ++i) {
            if (i & axis)   continue;
            if (asdf->branches[i|axis]) {
                new->branches[i] = asdf->branches[i|axis];
                asdf->branches[i|axis] = NULL;
            } else {
                new->branches[i] = split_cell(asdf->branches[i], neighbor, axis);
            }
        }
    }

    return new;
}


const int MERGE_LEAF   = 2;

_STATIC_
int can_merge(const ASDF* const asdf, const uint8_t axis)
{
    int result = true;

    if (asdf->branches[axis] == NULL)   return false;

    for (int b=0; b < 8; ++b) {
        if ((b & axis) || asdf->branches[b] == NULL)    continue;

        // Pick the octant-pair
        // In a successful merge, B will be collapsed into A
        ASDF* A = asdf->branches[b];
        ASDF* B = asdf->branches[b|axis];

        // If both are BRANCH, then we can only merge them by
        // re-arranging the tree topology.  This only works
        // if they split in the same way and not along the
        // target axis.
        if (A->state == BRANCH && B->state == BRANCH)
        {
            const uint8_t maskA =
                (A->branches[4] ? 4 : 0) |
                (A->branches[2] ? 2 : 0) |
                (A->branches[1] ? 1 : 0);
            const uint8_t maskB =
                (B->branches[4] ? 4 : 0) |
                (B->branches[2] ? 2 : 0) |
                (B->branches[1] ? 1 : 0);

            if (maskA != maskB || axis & maskA) return false;
        }

        // If they are both FILLED or EMPTY, we're happy.
        else if ((A->state == EMPTY && B->state == EMPTY) ||
                 (A->state == FILLED && B->state == FILLED))
        {
            continue;
        }

        // If we want to merge leaf cells, then give it a go.
        else if (A->state != BRANCH && B->state != BRANCH)
        {

            // Pull d values from the pair of cells
            float d[8];
            for (int c=0; c < 8; ++c) {
                d[c] = (c & axis) ? B->d[c] : A->d[c];
            }


            // Check the d values that lie on the boundary.
            for (int c=0; c < 8; ++c) {
                if (!(c & axis)) continue;

                const float x = (c & 4) ? A->X.upper : A->X.lower;
                const float y = (c & 2) ? A->Y.upper : A->Y.lower;
                const float z = (c & 1) ? A->Z.upper : A->Z.lower;

                const float distance = interpolate(
                    d, x, y, z,
                    A->X.lower, A->Y.lower, A->Z.lower,
                    B->X.upper, B->Y.upper, B->Z.upper
                );

                // If this corner can't be reconstructed accurately or the
                // interpolated result, has the wrong sign, don't do it.
                if (fabs(distance - A->d[c]) > EPSILON)     return false;
                if ((distance < 0) != (A->d[c] < 0))        return false;
            }
            result |= MERGE_LEAF;
        }

        // In all other cases, we can't merge.
        else    return false;
    }

    return result;
}

void simplify(ASDF* const asdf, const _Bool merge_leafs)
{
    if (!asdf || asdf->state != BRANCH)  return;

    // Arrange the axes so that the largest axis is (potentially) merged first
    int axes[3];
    const float dx = asdf->X.upper - asdf->X.lower,
                dy = asdf->Y.upper - asdf->Y.lower,
                dz = asdf->Z.upper - asdf->Z.lower;
    if (dx >= dy && dx >= dz) {
        axes[0] = 4;
        axes[1] = dy >= dz ? 2 : 1;
        axes[2] = dy >= dz ? 1 : 2;
    } else if (dy >= dx && dy >= dz) {
        axes[0] = 2;
        axes[1] = dx >= dz ? 4 : 1;
        axes[2] = dx >= dz ? 1 : 4;
    } else {
        axes[0] = 1;
        axes[1] = dx >= dy ? 4 : 2;
        axes[2] = dx >= dy ? 2 : 4;
    }

    // Loop over all three axes
    for (int a=0; a < 3; ++a) {
        int merge = can_merge(asdf, axes[a]);

        if (!merge || ((merge & MERGE_LEAF) && !merge_leafs)) {
            continue;
        }

        // Execute the merge
        for (int b=0; b < 8; ++b) {
            if (b & axes[a] || asdf->branches[b] == NULL) continue;

            ASDF* A = asdf->branches[b];
            ASDF* B = asdf->branches[b|axes[a]];

            merge_cells(A, B, axes[a]);
            free_asdf(B);
            asdf->branches[b|axes[a]] = NULL;
        }
    }

    // If we've merged all of the branches except one, switch the state
    // to something other than BRANCH.
    if (!asdf->branches[1] && !asdf->branches[2] && !asdf->branches[4]) {
        asdf->state = asdf->branches[0]->state;
        free_asdf(asdf->branches[0]);
        asdf->branches[0] = NULL;
    }
}


_STATIC_
void merge_cells(ASDF* const A, ASDF* const B, const int axis)
{
    A->X.upper = B->X.upper;
    A->Y.upper = B->Y.upper;
    A->Z.upper = B->Z.upper;
    for (int i=0; i < 8; ++i) {
        if (i & axis) {
            A->d[i] = B->d[i];
            A->branches[i] = B->branches[i & ~axis];
            B->branches[i & ~axis] = NULL;
        }
    }

    if (A->state == LEAF || B->state == LEAF) {
        A->state = LEAF;
    }
}

void simplify_recursive(ASDF* const asdf)
{
    if (!asdf || asdf->state != BRANCH) return;
    for (int i=0; i < 8; ++i)
        simplify_recursive(asdf->branches[i]);
    simplify(asdf, true);
}


////////////////////////////////////////////////////////////////////////////////

void asdf_scale(ASDF* const asdf, const float scale)
{
    if (asdf == NULL)   return;

    asdf->X.lower *= scale;
    asdf->X.upper *= scale;
    asdf->Y.lower *= scale;
    asdf->Y.upper *= scale;
    asdf->Z.lower *= scale;
    asdf->Z.upper *= scale;

    for (int i=0; i < 8; ++i)
        asdf_scale(asdf->branches[i], scale);
}

////////////////////////////////////////////////////////////////////////////////

ASDF* asdf_slice(const ASDF* const asdf, const float z)
{
    if (!asdf)  return NULL;

    ASDF* const out = calloc(1, sizeof(ASDF));
    *out = (ASDF){
        .state = BRANCH,
        .X = asdf->X,
        .Y = asdf->Y,
        .Z = (Interval){z, z}
    };

    if (asdf->state == LEAF) {
        out->state = LEAF;
        for (int i=0; i < 8; ++i) {
            out->d[i] = asdf_interpolate(asdf,
                i & 4 ? asdf->X.upper : asdf->X.lower,
                i & 2 ? asdf->Y.upper : asdf->Y.lower,
                z);
        }
    } else if (asdf->state == FILLED) {
        out->state = FILLED;
    } else if (asdf->state == EMPTY) {
        out->state = EMPTY;
    } else if (asdf->state == BRANCH) {
        uint8_t upper = asdf->branches[1] && z > asdf->branches[1]->Z.lower;
        for (int i=0; i < 8; i += 2) {
            out->branches[i] = asdf_slice(asdf->branches[i+upper], z);
        }
    }

    return out;
}

////////////////////////////////////////////////////////////////////////////////

void get_d_from_children(ASDF* const asdf)
{
    // Safely abort if this tree is incomplete
    if (!asdf || !asdf->branches[0]) return;

    const uint8_t axes = (asdf->branches[1] ? 1 : 0) |
                         (asdf->branches[2] ? 2 : 0) |
                         (asdf->branches[4] ? 4 : 0);


    for (int i=0; i < 8; ++i) {
        if (!asdf->branches[i&axes]) return;
        asdf->d[i] = asdf->branches[i&axes]->d[i];
    }
}


void get_xyz_from_children(ASDF* const asdf) {

    asdf->X = asdf->branches[0]->X;
    asdf->Y = asdf->branches[0]->Y;
    asdf->Z = asdf->branches[0]->Z;

    if (asdf->branches[1])
        asdf->Z.upper = asdf->branches[1]->Z.upper;
    if (asdf->branches[2])
        asdf->Y.upper = asdf->branches[2]->Y.upper;
    if (asdf->branches[4])
        asdf->X.upper = asdf->branches[4]->X.upper;
}


////////////////////////////////////////////////////////////////////////////////

void free_asdf(ASDF* const asdf)
{
    if (asdf == NULL)   return;

    if (asdf->state == BRANCH) {
        for (int i=0; i < 8; ++i) {
            free_asdf(asdf->branches[i]);
            asdf->branches[i] = NULL;
        }
    }

    free(asdf);
}

void free_virtual_asdf(ASDF* const asdf)
{
    if (asdf == NULL || asdf->state != VIRTUAL)     return;
    for (int b=0; b < 8; ++b) {
        free_virtual_asdf(asdf->branches[b]);
        asdf->branches[b] = NULL;
    }
    free(asdf);
}

ASDF* clone_virtual_asdf(const ASDF* const asdf)
{
    if (!asdf || asdf->state != VIRTUAL)  return (ASDF*)asdf;
    ASDF* const v = calloc(1, sizeof(ASDF));
    v->state = VIRTUAL;
    for (int b=0; b < 8; ++b) {
        v->branches[b] = clone_virtual_asdf(asdf->branches[b]);
    }
    return v;
}


////////////////////////////////////////////////////////////////////////////////

void free_data(ASDF* const asdf)
{
    if (!asdf)  return;

    free(asdf->data.vp);
    asdf->data.vp = NULL;

    for (int i=0; i < 8; ++i)   free_data(asdf->branches[i]);
}

////////////////////////////////////////////////////////////////////////////////

void asdf_verify(const ASDF* const asdf)
{
    if (!asdf)  return;
    if (asdf->state == BRANCH) {
        for (int i=0; i < 8; ++i)   asdf_verify(asdf->branches[i]);
    }
    else if (asdf->state == LEAF)   return;
    else {
        for (int d=0; d < 8; d++) {
            if ((asdf->d[d] < 0) ^ (asdf->state == FILLED)) {
                printf("Malformed ASDF error\n");
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

float asdf_sample(const ASDF* const asdf, const Vec3f p)
{
    if (asdf->state != BRANCH) {
        return asdf_interpolate(asdf, p.x, p.y, p.z);
    }

    const uint8_t branch =
        (p.x > asdf->branches[0]->X.upper ? 4 : 0) |
        (p.y > asdf->branches[0]->Y.upper ? 2 : 0) |
        (p.z > asdf->branches[0]->Z.upper ? 1 : 0);

    return asdf_sample(asdf->branches[branch], p);
}

////////////////////////////////////////////////////////////////////////////////

int count_cells(const ASDF* const asdf)
{
    if (asdf == NULL) {
        return 0;
    }

    int count = 1;
    for (int i=0; i < 8; ++i)   count += count_cells(asdf->branches[i]);

    return count;
}

int count_leafs(const ASDF* const asdf)
{
    if (!asdf) {
        return 0;
    } else if (asdf->state == LEAF) {
        return 1;
    } else if (asdf->state == BRANCH) {
        int count = 0;
        for (int i=0; i < 8; ++i)
            count += count_leafs(asdf->branches[i]);
        return count;
    } else {
        return 0;
    }
}

////////////////////////////////////////////////////////////////////////////////

int get_depth(const ASDF* const asdf)
{
    if (asdf == NULL) {
        return 0;
    }

    if (asdf->state == BRANCH) {
        int depth = 1;
        for (int i=0; i < 8; ++i) {
            int d = get_depth(asdf->branches[i]);
            if (d + 1 > depth)  depth = d+1;
        }
        return depth;
    }

    return 1;
}

////////////////////////////////////////////////////////////////////////////////

float asdf_get_max(const ASDF* const asdf)
{
    float max = -INFINITY;
    if (asdf == NULL) {
        return max;
    } else if (asdf->state == LEAF) {
        for (int i=0; i < 8; ++i) {
            max = fmax(max, asdf->d[i]);
        }
    } else if (asdf->state == BRANCH) {
        for (int i=0; i < 8; ++i) {
            max = fmax(max, asdf_get_max(asdf->branches[i]));
        }
    }

    return max;
}

float asdf_get_min(const ASDF* const asdf)
{
    float min = INFINITY;
    if (asdf == NULL) {
        return min;
    } else if (asdf->state == LEAF) {
        for (int i=0; i < 8; ++i) {
            min = fmin(min, asdf->d[i]);
        }
    } else if (asdf->state == BRANCH) {
        for (int i=0; i < 8; ++i) {
            min = fmin(min, asdf_get_min(asdf->branches[i]));
        }
    }

    return min;
}

////////////////////////////////////////////////////////////////////////////////

void find_dimensions(const ASDF* const asdf, int* ni, int* nj, int* nk)
{
    float dx = INFINITY, dy = INFINITY, dz = INFINITY;
    find_min_sizes(asdf, &dx, &dy, &dz);

    *ni = dx > 0 ? round((asdf->X.upper - asdf->X.lower)/dx) : 1;
    *nj = dy > 0 ? round((asdf->Y.upper - asdf->Y.lower)/dy) : 1;
    *nk = dz > 0 ? round((asdf->Z.upper - asdf->Z.lower)/dz) : 1;
}

_STATIC_
void find_min_sizes(const ASDF* const asdf, float* dx, float* dy, float* dz)
{
    if (asdf == NULL)   return;

    if (asdf->state == BRANCH) {
        for (int i=0; i < 8; ++i) {
            find_min_sizes(asdf->branches[i], dx, dy, dz);
        }
    } else {
        *dx = fmin(asdf->X.upper - asdf->X.lower, *dx);
        *dy = fmin(asdf->Y.upper - asdf->Y.lower, *dy);
        *dz = fmin(asdf->Z.upper - asdf->Z.lower, *dz);
    }
}

////////////////////////////////////////////////////////////////////////////////

float interpolate(const float d[8], float x, float y, float z,
                  float xmin, float ymin, float zmin,
                  float xmax, float ymax, float zmax)
{

    const float  xd = (xmin != xmax) ? (x - xmin) / (xmax - xmin) : 0.5;
    const float  yd = (ymin != ymax) ? (y - ymin) / (ymax - ymin) : 0.5;
    const float  zd = (zmin != zmax) ? (z - zmin) / (zmax - zmin) : 0.5;

    const float c00 = d[0]*(1-xd) + d[4]*xd;
    const float c01 = d[1]*(1-xd) + d[5]*xd;
    const float c10 = d[2]*(1-xd) + d[6]*xd;
    const float c11 = d[3]*(1-xd) + d[7]*xd;

    const float  c0 = c00*(1-yd) + c10*yd;
    const float  c1 = c01*(1-yd) + c11*yd;

    const float   c = c0*(1-zd) + c1*zd;

    return c;
}

float asdf_interpolate(const ASDF* const asdf,
                       const float x, const float y, const float z)
{
    return interpolate(
        asdf->d, x, y, z,
        asdf->X.lower, asdf->Y.lower, asdf->Z.lower,
        asdf->X.upper, asdf->Y.upper, asdf->Z.upper);
}


Vec3f asdf_zero_crossing(const ASDF* const leaf,
                         const uint8_t v0, const uint8_t v1)
{
    const float x0 = (v0 & 4) ? leaf->X.upper : leaf->X.lower,
                y0 = (v0 & 2) ? leaf->Y.upper : leaf->Y.lower,
                z0 = (v0 & 1) ? leaf->Z.upper : leaf->Z.lower;

    const float dx = ((v1 & 4) ? leaf->X.upper : leaf->X.lower) - x0,
                dy = ((v1 & 2) ? leaf->Y.upper : leaf->Y.lower) - y0,
                dz = ((v1 & 1) ? leaf->Z.upper : leaf->Z.lower) - z0;


    float d = 0.5;
    float step = 0.25;

    // Binary search along the edge to find the zero crossing
    // (because math is hard and for loops are easy)
    for (int iteration = 0; iteration < 16; ++iteration) {

        // Use interpolation to find the distance metric value at this edge
        const float result = asdf_interpolate(
            leaf, x0 + d*dx, y0 + d*dy, z0 + d*dz
        );

        // Change the binary search coefficient accordingly
        if (result < -EPSILON)         d += step;
        else if (result > EPSILON)     d -= step;
        else                        break;

        // And shrink the step size
        step /= 2;
    }

    return (Vec3f){x0+d*dx, y0+d*dy, z0+d*dz};
}


Vec3f asdf_gradient(const ASDF* const leaf,
                    const float x, const float y, const float z)
{
    const float xmin  = leaf->X.lower,
                ymin  = leaf->Y.lower,
                zmin  = leaf->Z.lower;

    const float dx    = leaf->X.upper - xmin,
                dy    = leaf->Y.upper - ymin,
                dz    = leaf->Z.upper - zmin;

    const float xd    = dx ? (x - xmin) / dx : 0.5,
                yd    = dy ? (y - ymin) / dy : 0.5,
                zd    = dz ? (z - zmin) / dz : 0.5;

    const float c000  = leaf->d[0], c001 = leaf->d[1],
                c010  = leaf->d[2], c011 = leaf->d[3],
                c100  = leaf->d[4], c101 = leaf->d[5],
                c110  = leaf->d[6], c111 = leaf->d[7];

    // Numerically find the gradients
    return (Vec3f){
        .x = -(zd - 1)*((yd - 1)*(c000/dx - c100/dx) - (c010/dx - c110/dx)*yd) + (z - zmin)*((yd - 1)*(c001/dx - c101/dx) - (c011/dx - c111/dx)*yd)/dz,

        .y = (zd - 1)*(((xd - 1)*c010 - xd*c110)/dy - ((xd - 1)*c000 - xd*c100)/dy) - (((xd - 1)*c011 - xd*c111)/dy - ((xd - 1)*c001 - xd*c101)/dy)*zd,

        .z = -((yd - 1)*((xd - 1)*c000 - xd*c100) - (y - ymin)*((xd - 1)*c010 - xd*c110)/dy)/dz + ((yd - 1)*((xd - 1)*c001 - xd*c101) - (y - ymin)*((xd - 1)*c011 - xd*c111)/dy)/dz
    };

    /*
    # Here is the python code used to find the formulas above
    # It should be run in sage, a python CAS environment
    var('c000, c001, c010, c011, c100, c101, c110, c111')
    var('x, y, z')
    var('xmin, ymin, zmin')
    var('dx, dy, dz')
    xd = (x - xmin) / dx
    yd = (y - ymin) / dy
    zd = (z - zmin) / dz

    c00  = c000*(1-xd) + c100*xd
    c01  = c001*(1-xd) + c101*xd
    c10  = c010*(1-xd) + c110*xd
    c11  = c011*(1-xd) + c111*xd
    c0  = c00*(1-yd) + c10*yd
    c1  = c01*(1-yd) + c11*yd
    c = c0*(1-zd) + c1*zd

    print '\n'.join(map(str, c.gradient((x,y,z))))
    */
}

void asdf_histogram(const ASDF* const asdf, int (*bins)[4], const int level)
{
    if (asdf == NULL)   return;
    bins[level][asdf->state]++;

    for (int i=0; i < 8; ++i) {
        asdf_histogram(asdf->branches[i], bins, level+1);
    }
}
/*

void flop(ASDF* const asdf)
{
    float d[8];
    ASDF* b[8];
    for (int i=0; i < 8; ++i) {
        d[i] = asdf->d[i];
        b[i] = asdf->branches[i];
    }

    Interval Y = asdf->Y;
    asdf->Y = asdf->Z;
    asdf->Z = Y;

    asdf->d[0] = d[1];
    asdf->d[1] = d[3];
    asdf->d[2] = d[0];
    asdf->d[3] = d[2];

    asdf->d[4] = d[5];
    asdf->d[5] = d[7];

*/
