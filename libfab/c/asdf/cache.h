#ifndef CACHE_H
#define CACHE_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "util/region.h"
#include "util/vec3f.h"

struct ASDF_;

/** @struct Corner_
    @brief A tree-based cache indexed by i, j, k
    position in a discrete lattice.

    @details
    Each cache has a depth value.  For any given i, j, k value, the
    corresponding cache corner is reached when bits below the depth-th
    bit (counting from the top) are all zeros.

    For example (showing only the top eight bits)\n
        i = 0b11010000, j = 0b10111000, k = 0b00000000\n
    will be at a depth of 5 (j is the limiting factor)
*/
typedef struct Corner_ {
    /** @var depth
        Cache recursion level */
    uint8_t depth;
    /** @var value
        Sample value (or NAN if uninitialized)*/
    float   value;

    /** @var branches
        Recursive branches (or NULL if uninitialized) */
    struct Corner_* branches[8];
} Corner;


/** @brief Recursively frees a corner cache
*/
void free_corner_cache(Corner* const cache);


/** @brief Writes a cache out to a file
    @details Cache values are discretized to 16 bit integers
*/
void  write_cache(const Corner* const cache, FILE* file);


/** @brief Reads a cache in from a file
*/
Corner* read_cache(FILE* file);



/** @brief Returns the corner corresponding to the given i, j, k position.
    @details The corner may be uninitialized (with value of NAN)
*/
Corner* get_corner(Corner* const cache,
                   const uint16_t i,
                   const uint16_t j,
                   const uint16_t k);


/** @brief Returns a subcache containing the given region
*/
Corner* corner_subcache(Corner* const cache,
                        const uint16_t imin, const uint16_t imax,
                        const uint16_t jmin, const uint16_t jmax,
                        const uint16_t kmin, const uint16_t kmax);


/** @brief Creates a populated cache with corner values of ASDF leaf cells.
*/
Corner* fill_corner_cache(const struct ASDF_* const asdf);

/** @brief Creates a populated cache with corner values of ASDF leaf, empty, and full cells.
*/
Corner* fill_corner_cache_all(const struct ASDF_* const asdf);

#endif
