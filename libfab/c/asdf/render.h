#ifndef RENDER_H
#define RENDER_H

#include "util/region.h"

// Forward declaration of ASDF structure
struct ASDF_;

////////////////////////////////////////////////////////////////////////////////

/** @brief Renders an ASDF to a height-map image
    @details (might be broken at the moment)
    @param asdf ASDF to render
    @param r_ Render region
    @param M Array of rotation parameters [cos(a), sin(a), cos(b), sin(b)]
    @param depth Height-map lattice to populate
*/
void render_asdf(const struct ASDF_* const asdf, const Region r_,
                 const float M[4], uint16_t*const*const depth);

////////////////////////////////////////////////////////////////////////////////

/** @brief Renders an ASDF to a height-map image, shaded, and normals image
    @param asdf ASDF to render
    @param r_ Render region (ni, nj must be lattice dimensions)
    @param M Array of rotation parameters [cos(a), sin(a), cos(b), sin(b)]
    @param depth Height-map lattice to populate
    @param shaded Shaded image to populate with shaded render
    @param normals RGB image to populate with colored normals
*/
void render_asdf_shaded(const struct ASDF_* const asdf, const Region r_,
                 const float M[4], uint16_t*const*const depth,
                 uint16_t*const*const shaded, uint8_t (**normals)[3]);

////////////////////////////////////////////////////////////////////////////////

/** @brief Draws the outline of ASDF cells on an image.
    @param a ASDF to render
    @param r Region to render (ni, nj must be lattice dimensions)
    @param img RGB image on which outlines will be drawn
*/
void draw_asdf_cells(ASDF* a, Region r, uint8_t (**img)[3]);

////////////////////////////////////////////////////////////////////////////////

#endif
