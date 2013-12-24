#ifndef IMPORT_H
#define IMPORT_H

#include <stdbool.h>

#include "util/region.h"

struct ASDF_;

/** @brief Converts a .vol file into an ASDF
    @param filename Target filename
    @param ni Number of samples on X axis
    @param nj Number of samples on Y axis
    @param nk Number of samples on Z axis
    @param offset Isosurface density value
    @param mm_per_voxel Voxel scale
    @param merge_leafs Boolean determining whether leaf cells are combined.
    @param close_border Boolean determining if border samples are set to zero.
*/
struct ASDF_* import_vol(const char* filename,
                         const int ni, const int nj, const int nk,
                         const float offset, const float mm_per_voxel,
                         const _Bool merge_leafs, const _Bool close_border);

/** @brief Imports a region within a .vol file
    @details Recurses until the region size will fill less than 100 MB in RAM, then loads the relevant samples from the file and recursively constructs tree.
    @param filename Target filename
    @param ni Number of samples (in full region) on X axis
    @param nj Number of samples (in full region) on Y axis
    @param nk Number of samples (in full region) on Z axis
    @param r Target region to import (should be within full region lattice)
    @param shift Sampling level
    @param offset Isosurface density value
    @param merge_leafs Boolean determining whether leaf cells are combined.
    @param close_border Boolean determining if border samples are set to zero.
*/
struct ASDF_* import_vol_region(
    const char* filename, const int ni, const int nj, const int nk,
    const Region r, const int shift, const float offset,
    const _Bool merge_leafs, const _Bool close_border);


/** @brief Imports a 2D lattice
    @param distances 2D array of distance samples
    @param ni Number of samples in X direction
    @param nj Number of samples in Y direction
    @param offset Isosurface distance values
    @param mm_per_pixel Lattice scale factor
    @param merge_leafs Boolean determining whether leaf cells are combined.
*/
struct ASDF_* import_lattice(float const*const*const distances,
                             const int ni, const int nj, const float offset,
                             const float mm_per_pixel, _Bool merge_leafs);


#endif
