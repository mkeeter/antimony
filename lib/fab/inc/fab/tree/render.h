#ifndef TREE_RENDER_H
#define TREE_RENDER_H

#include <stdint.h>

#include "fab/util/region.h"

#ifdef __cplusplus
extern "C" {
#endif

struct MathTree_;

/** @brief Recursively renders a tree
    @param tree Target tree
    @param region Region to render (ni, nj must be image dimensions)
    @param img Target image to populate
    @param halt Flag to abort (if *halt becomes true)
*/
void render8(struct MathTree_* tree, Region region,
             uint8_t** img, volatile int* halt,
             void (*callback)());

void shaded8(struct MathTree_* tree, Region region, uint16_t** depth,
             uint8_t (**out)[3], volatile int* halt,
             void (*callback)());

/** @brief Recursively renders a tree
    @param tree Target tree
    @param region Region to render (ni, nj must be image dimensions)
    @param img Target image to populate
    @param halt Flag to abort (if *halt becomes true)
*/
void render16(struct MathTree_* tree, Region region,
              uint16_t** img, volatile int* halt,
              void (*callback)());


#ifdef __cplusplus
}
#endif

#endif
