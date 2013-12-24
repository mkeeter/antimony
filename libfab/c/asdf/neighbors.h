#ifndef NEIGHBORS_H
#define NEIGHBORS_H

#include <stdint.h>

struct ASDF_;

/** @brief Returns a neighboring ASDF.
    @details The returned ASDF will exactly match the child ASDF's
    face.  If the child ASDF's face is a subset of another's face, then
    we return NULL.  If the child ASDF's face intersects another's face,
    returns a virtual ASDF with no branches and d = {split, nsplit, axis}.

    @param asdf Parent ASDF
    @param branch ID of child ASDF branch
    @param face ID of target face (in order -z, +z, -y, +y, -x, +x)
    @param neighbor Neighbor of parent ASDF on this face
*/
const ASDF* get_neighbor_v(
        const struct ASDF_* const asdf, const uint8_t branch,
        const uint8_t face, const struct ASDF_* const neighbor);

/** @brief Populates 'new' with the six neighbors of branch b of the asdf
    in the order -z, +z, -y, +y, -x, +x

    @details Neighbors are saved if they are at the same scale.  Virtual
    ASDFs are constructed to prevent subtle cracks in multiscale neighbors.

    @param asdf Top-level ASDF
    @param old Neighbors of top-level ASDF
    @param new Array to fill with new neighbors
    @param b Branch of top-level ASDF to examine (0-8)
*/
void get_neighbors_v(const struct ASDF_* asdf,
                     const struct ASDF_* const old[6],
                     const struct ASDF_* new[6], uint8_t b);


/** @brief Populates 'new' with the six neighbors of branch b of the asdf
    in the order -z, +z, -y, +y, -x, +x

    @details Neighbors are only saved if they are at the same scale,
    to prevent cracks.

    @param asdf Top-level ASDF
    @param old Neighbors of top-level ASDF
    @param new Array to fill with new neighbors
    @param b Branch of top-level ASDF to examine (0-8)
 */
void get_neighbors_3d(const struct ASDF_* asdf,
                      const struct ASDF_* const old[6],
                      const struct ASDF_* new[6], uint8_t b);


/** @brief Populates 'new' with the six neighbors of branch b of the asdf
    in the order -z, +z, -y, +y, -x, +x

    @details Neighbors at different scales are still recorded, because in 2D
    you can't get cracks from marching square (assuming a well-constructed
    distance tree).

    @param asdf Top-level ASDF
    @param old Neighbors of top-level ASDF
    @param new Array to fill with new neighbors
    @param b Branch of top-level ASDF to examine (0-8)
 */
void get_neighbors_2d(const struct ASDF_* asdf,
                      const struct ASDF_* const old[4],
                      const struct ASDF_* new[4], uint8_t b);


#endif
