#include <stdlib.h>
#include <math.h>

#include "asdf/asdf.h"
#include "asdf/cache.h"

#include "util/region.h"
#include "util/macros.h"

/* Forward declarations */

_STATIC_
void _write_cache(const Corner* const cache, FILE* file,
                  const float scale);

_STATIC_
Corner* _read_cache(FILE* file, const int depth, const float scale);

_STATIC_
void get_range(const Corner* const cache, float* const min, float* const max);


_STATIC_
void   _fill_corner_cache(const struct ASDF_* const asdf,
                   Corner* const cache, const Region r);

_STATIC_
void _fill_corner_cache_all(
    const ASDF* const asdf, Corner* const cache, const Region r);

////////////////////////////////////////////////////////////////////////////////
//      Corner cache functions
////////////////////////////////////////////////////////////////////////////////

void free_corner_cache(Corner* const cache)
{
    if (!cache) return;
    for (int i=0; i < 8; ++i)
        free_corner_cache(cache->branches[i]);
    free(cache);
}

////////////////////////////////////////////////////////////////////////////////

void write_cache(const Corner* const cache, FILE* file)
{
    float min = INFINITY, max = -INFINITY;

    get_range(cache, &min, &max);

    // We'll get fifteen bits of resolution on each side
    float scale = 32767;
    if (fabs(max) > fabs(min))  scale /= fabs(max);
    else                        scale /= fabs(min);

    for (int i=0; i < 4; ++i) fputc( ((char*)&scale)[i], file);

    _write_cache(cache, file, scale);
}


_STATIC_
void _write_cache(const Corner* const cache, FILE* file, const float scale)
{
    if (!cache) return;

    // Discretize the sampled value into the 16-bit range
    float scaled = cache->value * scale;
    int16_t v = BOUND(scaled, -32768, 32767);
    if (v == 0 && cache->value < 0)     v = -1;

    // Write the discretized value to file
    for (int i=0; i < 2; ++i) fputc( ((char*)&v)[i], file);

    // Write a single character stating which branches are active
    uint8_t branching = 0;
    for (int i=0; i < 8; ++i) {
        if (cache->branches[i]) {
            branching |= (1 << i);
        }
    }
    fputc(branching, file);

    // Write the cache branches
    for (int i=0; i < 8; ++i) {
        _write_cache(cache->branches[i], file, scale);
    }
}

Corner* read_cache(FILE* file)
{
    float scale;
    fscanf(file, "%4c", ((char*)&scale));

    return _read_cache(file, 0, scale);
}

_STATIC_
Corner* _read_cache(FILE* file, const int depth, const float scale)
{
    Corner* cache = calloc(1, sizeof(Corner));
    cache->depth = depth;

    // Read in the sampled value.
    int16_t v;
    fscanf(file, "%2c", ((char*)&v));
    cache->value = v / scale;

    // Read in the branching factor
    char branching = fgetc(file);

    // Read in the individual branches
    for (int i=0; i < 8; ++i) {
        if (branching & (1 << i)) {
            cache->branches[i] = _read_cache(file, depth+1, scale);
        }
    }

    return cache;
}

_STATIC_
void get_range(const Corner* const cache, float* const min, float* const max)
{
    if (cache == NULL)  return;

    if (!isnan(cache->value)) {
        if (cache->value < *min)    *min = cache->value;
        if (cache->value > *max)    *max = cache->value;
    }

    for (int i=0; i < 8; ++i) {
        get_range(cache->branches[i], min, max);
    }
}

////////////////////////////////////////////////////////////////////////////////

Corner* get_corner(Corner* cache, const uint16_t i,
                   const uint16_t j, const uint16_t k)
{
    uint16_t mask = (1 << (16 - cache->depth)) - 1;

    // Check whether all bits lower than depth are zero
    if ( ((i | j | k) & mask) == 0)
        return cache;

    mask = 1 << (15 - cache->depth);
    uint8_t index = ((mask & i) ? 4 : 0) +
                    ((mask & j) ? 2 : 0) +
                    ((mask & k) ? 1 : 0);

    if (cache->branches[index] == NULL) {
        cache->branches[index] = calloc(1, sizeof(Corner));
        cache->branches[index]->depth = cache->depth+1;
        cache->branches[index]->value = NAN;
    }
    return get_corner(cache->branches[index], i, j, k);
}

////////////////////////////////////////////////////////////////////////////////

Corner* corner_subcache(Corner* cache,
                        const uint16_t imin, const uint16_t imax,
                        const uint16_t jmin, const uint16_t jmax,
                        const uint16_t kmin, const uint16_t kmax)
{
    uint16_t mask = (1 << (16 - cache->depth)) - 1;

    // Check whether all bits lower than depth are zero
    if ( ((imin | jmin | kmin) & mask) == 0)
        return cache;

    mask = 1 << (15 - cache->depth);

    // These values are non-zero if the desired bit matches, 0 otherwise.
    uint16_t i = imin ^ imax;
    uint16_t j = jmin ^ jmax;
    uint16_t k = kmin ^ kmax;

    // If any of these values don't match, then we can't go any
    // further down the cache tree.
    if ((i | j | k) & mask)     return cache;

    uint8_t index = ((mask & imin) ? 4 : 0) +
                    ((mask & jmin) ? 2 : 0) +
                    ((mask & kmin) ? 1 : 0);

    // If the desired subcache doesn't exist, then make it.
    if (cache->branches[index] == NULL) {
        cache->branches[index] = calloc(1, sizeof(Corner));
        cache->branches[index]->depth = cache->depth+1;
        cache->branches[index]->value = NAN;
    }

    // And recurse further down the tree.
    return corner_subcache(cache->branches[index],
                           imin, imax, jmin, jmax, kmin, kmax);

}

////////////////////////////////////////////////////////////////////////////////

Corner* fill_corner_cache(const ASDF* const asdf) {
    int ni, nj, nk;
    find_dimensions(asdf, &ni, &nj, &nk);

    Region r = (Region){.imin = 0,  .jmin = 0,  .kmin = 0,
                        .ni   = ni, .nj   = nj, .nk   = nk,
                        .voxels = ni*nj*nk
                        };
    Corner* cache = calloc(1, sizeof(Corner));
    cache->value = NAN;

    _fill_corner_cache(asdf, cache, r);

    return cache;
}


_STATIC_
void _fill_corner_cache(const ASDF* const asdf, Corner* const cache, const Region r)
{
    if (asdf == NULL)   return;

    if (asdf->state == BRANCH) {

        // Get a subcache to speed up cache indexing
        Corner* subcache = corner_subcache(cache,
                                           r.imin, r.imin+r.ni,
                                           r.jmin, r.jmin+r.nj,
                                           r.kmin, r.kmin+r.nk);

        Region octants[8];
        octsect_merged(r, asdf, octants);

        // Recurse down into the ASDF tree
        for (int a=0; a < 8; ++a) {
            _fill_corner_cache(asdf->branches[a], subcache, octants[a]);
        }

    } else if (asdf->state == LEAF) {

        // If this is a leaf, full, or empty cell, we'll save
        // the corner values in the cache.
        for (int a=0; a < 8; ++a) {
            uint16_t i = (a & 4) ? r.imin + r.ni : r.imin;
            uint16_t j = (a & 2) ? r.jmin + r.nj : r.jmin;
            uint16_t k = (a & 1) ? r.kmin + r.nk : r.kmin;

            Corner* c = get_corner(cache, i, j, k);
            c->value = asdf->d[a];
        }
    }
}

////////////////////////////////////////////////////////////////////////////////



Corner* fill_corner_cache_all(const ASDF* const asdf) {
    int ni, nj, nk;
    find_dimensions(asdf, &ni, &nj, &nk);

    Region r = (Region){.imin = 0,  .jmin = 0,  .kmin = 0,
                        .ni   = ni, .nj   = nj, .nk   = nk,
                        .voxels = ni*nj*nk
                        };
    Corner* cache = calloc(1, sizeof(Corner));
    cache->value = NAN;

    _fill_corner_cache_all(asdf, cache, r);

    return cache;
}


_STATIC_
void _fill_corner_cache_all(
    const ASDF* const asdf, Corner* const cache, const Region r)
{
    if (asdf == NULL)   return;

    if (asdf->state == BRANCH) {

        // Get a subcache to speed up cache indexing
        Corner* subcache = corner_subcache(cache,
                                           r.imin, r.imin+r.ni,
                                           r.jmin, r.jmin+r.nj,
                                           r.kmin, r.kmin+r.nk);

        Region octants[8];
        octsect_merged(r, asdf, octants);

        // Recurse down into the ASDF tree
        for (int a=0; a < 8; ++a) {
            _fill_corner_cache_all(asdf->branches[a], subcache, octants[a]);
        }

    } else {
        // If this is a leaf, full, or empty cell, we'll save
        // the corner values in the cache.
        for (int a=0; a < 8; ++a) {
            uint16_t i = (a & 4) ? r.imin + r.ni : r.imin;
            uint16_t j = (a & 2) ? r.jmin + r.nj : r.jmin;
            uint16_t k = (a & 1) ? r.kmin + r.nk : r.kmin;

            Corner* c = get_corner(cache, i, j, k);
            c->value = asdf->d[a];
        }
    }
}
