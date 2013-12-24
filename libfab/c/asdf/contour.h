#ifndef CONTOUR_H
#define CONTOUR_H

struct ASDF_;
struct Path_;

/** @brief Finds ASDF contours
    @details paths can be dereferenced to get an array of path pointers
    It may be reallocated to increase the storage size.
    @param asdf ASDF to contour
    @param paths Pointer to an array of path pointers
    @returns The number of paths stored.
*/
int contour(struct ASDF_* const asdf,
            struct Path_*** const paths, volatile int* const halt);

#endif
