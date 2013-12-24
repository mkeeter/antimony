#ifndef DISTANCE_H
#define DISTANCE_H

struct ASDF_*;

/** @brief Offsets an ASDF by the given distance
    @param asdf ASDF to offset
    @param offset Distance to offset (in mm, which are the ASDFs native units)
    @param pixels_per_mm Lattice resolution to use while offsetting
    @returns An offset ASDF
*/
struct ASDF_* asdf_offset(
    const struct ASDF_* const asdf, float offset, float pixels_per_mm);

#endif
