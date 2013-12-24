#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "tree/eval.h"
#include "tree/packed.h"
#include "tree/render.h"

#include "util/switches.h"

/*  region8
 *
 *  Renders a tree pixel-by-pixel into a given region,
 *  using the eval_r function to find an array of results in
 *  a single pass through the tree.
 *
 */
_STATIC_
void region8(PackedTree* tree, Region region, uint8_t** img);

/*  region16
 *
 *  Renders a tree pixel-by-pixel into a given region,
 *  using the eval_r function to find an array of results in
 *  a single pass through the tree.
 *
 */
_STATIC_
void region16(PackedTree* tree, Region region, uint16_t** img);

////////////////////////////////////////////////////////////////////////////////
void render8(PackedTree* tree, Region region,
             uint8_t** img, volatile int* halt)
{
    // Special interrupt system, set asynchronously by on high
    if (*halt)  return;

    // Render pixel-by-pixel if we're below a certain size.
    if (region.voxels > 0 && region.voxels < MIN_VOLUME) {
        region8(tree, region, img);
        return;
    }


    // Pre-emptively halt evaluation if all the points in this
    // region are already light.
    uint8_t L = region.L[region.nk] >> 8;
    bool cull = true;
    for (int row = region.jmin; cull && row < region.jmin + region.nj; ++row) {
        for (int col = region.imin; cull && col < region.imin + region.ni; ++col) {
            if (L > img[row][col]) {
                cull = false;
                break;
            }
        }
    }
    if (cull) return;

    Interval X = {region.X[0], region.X[region.ni]},
             Y = {region.Y[0], region.Y[region.nj]},
             Z = {region.Z[0], region.Z[region.nk]};

    Interval result = eval_i(tree, X, Y, Z);

    // If we're inside the object, fill with color.
    if (result.upper < 0) {
        for (int row = region.jmin; row < region.jmin + region.nj; ++row) {
            for (int col = region.imin; col < region.imin + region.ni; ++col) {
                if (L > img[row][col])  img[row][col] = L;
            }
        }
    }

    // In unambiguous cases, return immediately
    if (result.upper < 0 || result.lower >= 0)  return;

#if PRUNE
    disable_nodes(tree);
    disable_nodes_binary(tree);
#endif

    // Subdivide and recurse if we're not at voxel size.
    if (region.ni*region.nj*region.nk > 1) {
        Region A, B;

        bisect(region, &A, &B);

        render8(tree, B, img, halt);
        render8(tree, A, img, halt);
    }

#if PRUNE
    enable_nodes(tree);
#endif

}

////////////////////////////////////////////////////////////////////////////////

_STATIC_
void region8(PackedTree* tree, Region region, uint8_t** img)
{
    float *X = malloc(region.voxels*sizeof(float)),
          *Y = malloc(region.voxels*sizeof(float)),
          *Z = malloc(region.voxels*sizeof(float));

    // Copy the X, Y, Z vectors into a flattened matrix form.
    int q = 0;
    for (int k = region.nk - 1; k >= 0; --k) {
        for (int j = 0; j < region.nj; ++j) {
            for (int i = 0; i < region.ni; ++i) {
                X[q] = region.X[i];
                Y[q] = region.Y[j];
                Z[q] = region.Z[k];
                q++;
            }
        }
    }
    region.X = X;
    region.Y = Y;
    region.Z = Z;

    float* result = eval_r(tree, region);

    // Free the allocated matrices
    free(X);
    free(Y);
    free(Z);

    for (int k = region.nk - 1; k >= 0; --k) {
        uint8_t L = region.L[k+1] >> 8;

        for (int j = 0; j < region.nj; ++j) {
            int row = j + region.jmin;

            for (int i = 0; i < region.ni; ++i) {
                int col = i + region.imin;

               if (*(result++) < 0 && img[row][col] < L) {
                    img[row][col] = L;
                }
            }
        }
    }
}




////////////////////////////////////////////////////////////////////////////////
void render16(PackedTree* tree, Region region,
              uint16_t** img, volatile int* halt)
{
    if (tree == NULL)  return;

    // Special interrupt system, set asynchronously by on high
    if (*halt)  return;

    // Render pixel-by-pixel if we're below a certain size.
    if (region.voxels > 0 && region.voxels < MIN_VOLUME) {
        region16(tree, region, img);
        return;
    }

    // Pre-emptively halt evaluation if all the points in this
    // region are already light.
    uint16_t L = region.L[region.nk];
    bool cull = true;
    for (int row = region.jmin; cull && row < region.jmin + region.nj; ++row) {
        for (int col = region.imin; cull && col < region.imin + region.ni; ++col) {
            if (L > img[row][col]) {
                cull = false;
                break;
            }
        }
    }
    if (cull) return;

    Interval X = {region.X[0], region.X[region.ni]},
             Y = {region.Y[0], region.Y[region.nj]},
             Z = {region.Z[0], region.Z[region.nk]};

    Interval result = eval_i(tree, X, Y, Z);

    // If we're inside the object, fill with color.
    if (result.upper < 0) {
        for (int row = region.jmin; row < region.jmin + region.nj; ++row) {
            for (int col = region.imin; col < region.imin + region.ni; ++col) {
                if (L > img[row][col])  img[row][col] = L;
            }
        }
    }

    // In unambiguous cases, return immediately
    if (result.upper < 0 || result.lower >= 0)  return;

#if PRUNE
    disable_nodes(tree);
    disable_nodes_binary(tree);
#endif

    // Subdivide and recurse if we're not at voxel size.
    if (region.ni*region.nj*region.nk > 1) {
        Region A, B;
        bisect(region, &A, &B);

        render16(tree, B, img, halt);
        render16(tree, A, img, halt);
    }

#if PRUNE
    enable_nodes(tree);
#endif

}

////////////////////////////////////////////////////////////////////////////////

_STATIC_
void region16(PackedTree* tree, Region region, uint16_t** img)
{
    float *X = malloc(region.voxels*sizeof(float)),
          *Y = malloc(region.voxels*sizeof(float)),
          *Z = malloc(region.voxels*sizeof(float));

    // Copy the X, Y, Z vectors into a flattened matrix form.
    int q = 0;
    for (int k = region.nk - 1; k >= 0; --k) {
        for (int j = 0; j < region.nj; ++j) {
            for (int i = 0; i < region.ni; ++i) {
                X[q] = region.X[i];
                Y[q] = region.Y[j];
                Z[q] = region.Z[k];
                q++;
            }
        }
    }
    region.X = X;
    region.Y = Y;
    region.Z = Z;

    float* result = eval_r(tree, region);

    // Free the allocated matrices
    free(X);
    free(Y);
    free(Z);

    for (int k = region.nk - 1; k >= 0; --k) {
        uint16_t L = region.L[k+1];

        for (int j = 0; j < region.nj; ++j) {
            int row = j + region.jmin;

            for (int i = 0; i < region.ni; ++i) {
                int col = i + region.imin;

               if (*(result++) < 0 && img[row][col] < L) {
                    img[row][col] = L;
                }
            }
        }
    }
}
