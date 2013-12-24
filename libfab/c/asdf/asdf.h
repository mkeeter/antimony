#ifndef ASDF_H
#define ASDF_H

#include <stdint.h>
#include <stdbool.h>

#include "util/interval.h"
#include "util/region.h"
#include "util/vec3f.h"

struct PackedTree_;
struct Corner_;

struct Path_;
struct CMSpath_;

////////////////////////////////////////////////////////////////////////////////

enum ASDFstate { FILLED, EMPTY, BRANCH, LEAF, VIRTUAL };

/** @struct ASDF_
    @brief ASDF tree node
*/


typedef struct ASDF_ {
    /** @var state
        ASDF cell type (FILLED, EMPTY, BRANCH, or LEAF) */
    enum ASDFstate state;
    /** @var X
        X bounds of ASDF cell*/
    /** @var Y
        Y bounds of ASDF cell*/
    /** @var Z
        Z bounds of ASDF cell */

    Interval X, Y, Z;

    /** @var branches
        Array of branches (with NULLs) */
    struct ASDF_* branches[8];

    /** @var d
        Array of distance samples */
    float d[8];

    /** @var data
        Pointer to situation-dependent data */
    union {
        void* vp;
        struct CMSpath_* (*cms)[4];
        uint32_t* tri;
        struct Path_** contour;
    } data;
} ASDF;


/** @brief Converts a PackedTree into an ASDF data structure
    @param tree PackedTree
    @param region Region on which to render the expression
    @param merge_leafs Boolean determining whether leaf cells are merged
    @param halt Integer that should be set to 1 to abort render
*/
ASDF*  build_asdf(
    struct PackedTree_* const tree, const Region region,
    const _Bool merge_leafs, volatile int* const halt);


/** @brief Verifies that all corner signs are correct
    @details Prints an error message if there's a non-negative corner
    in a FILLED cell or a negative corner in an EMPTY cell.
*/
void asdf_verify(const ASDF* const asdf);


/** @brief Returns the minimum d value in the ASDF
*/
float asdf_get_min(const ASDF* const asdf);


/** @brief Returns the maximum d value in the ASDF
*/
float asdf_get_max(const ASDF* const asdf);


/** @brief Returns the height of an ASDF
 */
int get_depth(const ASDF* const asdf);


/** @brief Generates a root for an ASDF
    @details Evaluates the tree at region corners but does not recurse
*/
ASDF* asdf_root(struct PackedTree_* tree, Region region);


/** @brief Recursively frees an ASDF data structure
*/
void free_asdf(ASDF* asdf);


/** @brief Recursively frees a virtual ASDF data structure
*/
void free_virtual_asdf(ASDF* asdf);


/** @brief Recursively clones a virtual ASDF data structure
*/
ASDF* clone_virtual_asdf(const ASDF* const asdf);


/** @brief Recursively frees and NULLs the data arrays in an ASDF.
*/
void free_data(ASDF* asdf);


/** @brief Samples a single point within an ASDF
    @details Recurses until we find a leaf / empty / full cell that contains
    the given position, then uses trilinear interpolation to find a value.
*/
float asdf_sample(const ASDF* const asdf, const Vec3f p);


/** @brief Performs trilinear interpolation
    @param d Samples at all eight corners, arranged in increasing binary order
*/
float interpolate(const float d[8], const float x, const float y, const float z,
                  const float xmin, const float ymin, const float zmin,
                  const float xmax, const float ymax, const float zmax);


/** @brief Finds an interpolated distance value in a single ASDF cell.
*/
float asdf_interpolate(const ASDF* const asdf, const float x,
                       const float y, const float z);


/** @brief Finds a zero crossing between two corners of a leaf cell
    @param v0 First corner (0-7)
    @param v1 Second corner (0-7)
    @returns Position in 3D space
*/
Vec3f asdf_zero_crossing(const ASDF* const asdf,
                         const uint8_t v0, const uint8_t v1);


/** @brief Numerically finds the gradient at a point inside an ASDF cell.
*/
Vec3f asdf_gradient(const ASDF* const leaf, const float x,
                    const float y, const float z);


/** @brief Simplifies an ASDF cell by merging children non-recursively
    @param asdf Target ASDF
    @param merge_leafs If true, merge leaf cells as well as full/empty cells
*/
void simplify(ASDF* const asdf, const _Bool merge_leafs);

/** @brief Splits an ASDF cell along the given axis.
    @details The input asdf is modified in-place to become the lower half
    of the split.
    @returns The upper cell of the split
*/
ASDF* split_cell(ASDF* const asdf, const ASDF* neighbor, const uint8_t axis);

/** @brief Finds the number of voxels along each dimension
    @param ni Target for i voxel count
    @param nj Target for j voxel count
    @param nk Target for k voxel count
    @details Sometimes not accurate due to floating-point rounding.  Sorry.
*/
void find_dimensions(const ASDF* const asdf, int* ni, int* nj, int* nk);


/** @brief  Returns cell count in this ASDF
*/
int count_cells(const ASDF* const asdf);


/** @brief Returns the number of leaf cells in this ASDF
*/
int count_leafs(const ASDF* const asdf);


/**  @brief Scales the ASDf dimensions by the given scale factor
*/
void asdf_scale(ASDF* const asdf, const float scale);


/** @brief Slices an ASDF at the given z level
    @returns A flat (xy plane) ASDF with appropriate samples
*/
ASDF* asdf_slice(const ASDF* const old, const float z);


/** @brief Fills the values of asdf->d[i] with d samples from its children.
*/
void get_d_from_children(ASDF* const asdf);


/** @brief Fills x, y, and z intervals with values claimed from children.
 */
void get_xyz_from_children(ASDF* const asdf);


/** @brief Fills a set of bins with the number of cells at that tree level
    @param bins Array of appropriate size (number of ASDF levels), initialized to all zeros
    @param level 0 for top-level invocation
*/
void asdf_histogram(const ASDF* const asdf, int (*bins)[4], const int level);

////////////////////////////////////////////////////////////////////////////////

#endif
