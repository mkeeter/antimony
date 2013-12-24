#ifndef TREE_RENDER_H
#define TREE_RENDER_H

#include <stdint.h>

#include "util/region.h"

struct PackedTree_;

/** @brief Recursively renders a tree
    @param tree Target tree
    @param region Region to render (ni, nj must be image dimensions)
    @param img Target image to populate
    @param halt Flag to abort (if *halt becomes true)
*/
void render8(struct PackedTree_* tree, Region region,
             uint8_t** img, volatile int* halt);



/** @brief Recursively renders a tree
    @param tree Target tree
    @param region Region to render (ni, nj must be image dimensions)
    @param img Target image to populate
    @param halt Flag to abort (if *halt becomes true)
*/
void render16(struct PackedTree_* tree, Region region,
             uint16_t** img, volatile int* halt);


#endif
