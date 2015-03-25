#ifndef UTIL_REGION_H
#define UTIL_REGION_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct MathTree_;

typedef struct Region_ {
    uint32_t imin, jmin, kmin;
    uint32_t ni, nj, nk;
    uint64_t voxels;

    // Real-space coordinate arrays
    float *X, *Y, *Z;

    // Luminosity array (same size as Z)
    uint16_t* L;
} Region;


/*  build_arrays
 *
 *  Populates the X, Y, Z, and L arrays of a region.
 */
void build_arrays(Region* const R,
                  const float xmin, const float ymin, const float zmin,
                  const float xmax, const float ymax, const float zmax);


/*  free_arrays
 *
 *  Frees the X, Y, Z, and L arrays of a region
 */
void free_arrays(Region* const R);


/*  bisect
 *
 *  Splits a region in two along its longest axis.
 *  Returns 1 if the region is of volume 1 and can't be split,
 *  0 otherwise.
 */
int bisect(const Region r, Region* const A, Region* const B);

/*  octsect
 *
 *  Splits a region in two along all three axes if possible, storing
 *  results in the array 'out'.  Returns a bitfield with bits set
 *  in the positions where a region was stored.
 */
uint8_t octsect(const Region R, Region* const out);


/** @brief Splits a region along each active axis of the given MathTree
    @returns Bit mask of newly populated regions.
*/
int octsect_active(const Region r, const struct MathTree_* tree,
       Region* const out);


/*  split
 *
 *  Repeatedly cut a region in half along its longest axis to
 *  create some number of subregions, stored in the array 'out'.
 */
int split(const Region R, Region* const out, const int count);


/*  split_xy
 *
 *  Repeatedly cut a region in half along the X or Y axis to
 *  create some number of subregions, stored in the array 'out'.
 */
int split_xy(const Region R, Region* const out, const int count);


/*  bisect_{x,y,z}
 *
 *  Bisects a region along one axis.
 */
void bisect_x(const Region r, Region* const A, Region* const B);
void bisect_y(const Region r, Region* const A, Region* const B);
void bisect_z(const Region r, Region* const A, Region* const B);

#ifdef __cplusplus
}
#endif

#endif
