#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "asdf/asdf.h"
#include "asdf/import.h"

#include "util/region.h"

////////////////////////////////////////////////////////////////////////////////
// Forward declarations

/*  build_from_vol
 *
 *  Recursively constructs an ASDF from a set of density values.
 */
_STATIC_
ASDF* build_from_vol(
    float*** const data, const Region region,
    const _Bool merge_leafs);

_STATIC_
ASDF* build_from_lattice(
    float const*const*const data,
    const Region region, const float offset,
    const _Bool merge_leafs);

_STATIC_
ASDF* _import_vol_region(
    const char* filename,
    const int ni, const int nj, const int nk,
    const Region r, const Region full,
    const int shift, const float offset,
    const _Bool merge_leafs, const _Bool close_border);

////////////////////////////////////////////////////////////////////////////////


ASDF* import_vol(
    const char* filename, const int ni, const int nj, const int nk,
    const float offset, const float mm_per_voxel,
    const _Bool merge_leafs, const _Bool close_border)
{

    Region r = (Region){
        .imin = 0,  .jmin = 0,  .kmin = 0,
        .ni = ni-1, .nj = nj-1, .nk = nk-1,
        .voxels = (ni-1)*(nj-1)*(nk-1)
    };
    build_arrays(
        &r, 0, 0, 0,
        mm_per_voxel*ni, mm_per_voxel*nj, mm_per_voxel*nk
    );

    ASDF* const out = import_vol_region(
        filename, ni, nj, nk, r, 0, offset, merge_leafs, close_border
    );

    free_arrays(&r);
    return out;
}

ASDF* import_vol_region(
    const char* filename,
    const int ni, const int nj, const int nk,
    const Region r,
    const int shift, const float offset,
    const _Bool merge_leafs, const _Bool close_border)
{
    return _import_vol_region(
        filename, ni, nj, nk, r, r, shift, offset, merge_leafs, close_border
    );
}

_STATIC_
ASDF* _import_vol_region(
    const char* filename,
    const int ni, const int nj, const int nk,
    const Region r, const Region full,
    const int shift, const float offset,
    const _Bool merge_leafs, const _Bool close_border)
{

    printf(
        "Importing region (%i %i), (%i %i), (%i %i) with shift %i\n",
        r.imin, r.imin+r.ni, r.jmin, r.jmin+r.nj, r.kmin, r.kmin+r.nk, shift
    );

    // Don't allocate more than 100 MB of RAM for the arrays
    if ((r.ni*r.nj*r.nk*4 >> (shift*3)) > 100e6) {
        printf("Subdividing!\n");

        ASDF* const asdf = calloc(1, sizeof(ASDF));
        *asdf = (ASDF) {
            .state = BRANCH,
            .X = (Interval){r.X[0], r.X[r.ni]},
            .Y = (Interval){r.Y[0], r.Y[r.nj]},
            .Z = (Interval){r.Z[0], r.Z[r.nk]}
        };

        Region octants[8];
        uint8_t bits = octsect(r, octants);
        for (int i=0; i < 8; ++i) {
            if (bits & (1<<i)) {
                asdf->branches[i] = _import_vol_region(
                    filename, ni, nj, nk,
                    octants[i], full, shift, offset,
                    merge_leafs, close_border
                );
            }
        }
        get_d_from_children(asdf);
        simplify(asdf, merge_leafs);

        return asdf;

    } else {

        // Allocate space for data we're about to load from the file
        float*** data = malloc(sizeof(float**)*((r.nk>>shift)+1));
        for (int k=0; k < (r.nk>>shift)+1; ++k) {
            data[k] = malloc(sizeof(float*)*((r.nj>>shift)+1));
            for (int j=0; j < (r.nj>>shift)+1; ++j) {
                data[k][j] = calloc((r.ni>>shift)+1, sizeof(float*));
            }
        }

        const int mask = ~((1 << shift) - 1);
        // Load region of data into the resampled data array
        FILE* file = fopen(filename, "rb");
        for (int k=r.kmin & mask; k <= (r.kmin & mask)+r.nk; k += (1 << shift)) {
            for (int j=r.jmin & mask; j <= (r.jmin & mask)+r.nj; j += (1 << shift)) {

                // Seek to the start of this data block
                fseek(
                    file,
                    4*(k*ni*nj + j*ni + (r.imin & mask)),
                    SEEK_SET
                );

                for (int i=r.imin & mask; i <= (r.imin & mask)+r.ni; i += (1 << shift)) {
                    float sample;
                    fscanf(file, "%4c", (char*)&sample);

                    if (close_border && (
                            i <= full.imin ||
                            i >= ((full.imin + full.ni) & mask) ||
                            j <= full.jmin ||
                            j >= ((full.jmin + full.nj) & mask) ||
                            k <= full.kmin ||
                            k >= ((full.kmin + full.nk) & mask))
                        )
                    {
                        sample = 0;
                    }

                    data[(k-(r.kmin & mask))>>shift]
                        [(j-(r.jmin & mask))>>shift]
                        [(i-(r.imin & mask))>>shift] = -sample + offset;

                    // Skip unsampled points.
                    for (int a=0; a < 4*((1 << shift) - 1); ++a) {
                        getc(file);
                    }
                }
            }
        }
        fclose(file);


        // Construct a region with {ijk}min=0 and a downsampled number of XYZ positions
        // so that build_from_vol indexes at the right point in the data array.
        Region _r = (Region){
            .imin = 0, .jmin = 0, .kmin = 0,
            .ni = r.ni >> shift, .nj = r.nj >> shift, .nk = r.nk >> shift,
        };
        build_arrays(
            &_r, r.X[0], r.Y[0], r.Z[0],
            r.X[r.ni], r.Y[r.nj] ,r.Z[r.nk]
        );
        // Eat the data and return an ASDF
        ASDF* const asdf = build_from_vol(data, _r, merge_leafs);

        // FREE ALL THE DATA!
        for (int k=0; k < (r.nk>>shift)+1; ++k) {
            for (int j=0; j < (r.nj>>shift)+1; ++j) {
                free(data[k][j]);
            }
            free(data[k]);
        }
        free(data);
        free_arrays(&_r);

        printf(
            "    Loaded %i cells, %i leafs\n",
            count_cells(asdf), count_leafs(asdf)
        );

        return asdf;
    }
}



_STATIC_
ASDF* build_from_vol(float*** const data, const Region region,
                            const _Bool merge_leafs)
{
    ASDF* asdf = calloc(1, sizeof(ASDF));

    *asdf = (ASDF) {
        .state = BRANCH,
        .X = (Interval){region.X[0], region.X[region.ni]},
        .Y = (Interval){region.Y[0], region.Y[region.nj]},
        .Z = (Interval){region.Z[0], region.Z[region.nk]}
    };

    // Scan across samples to see if this cell is filled or empty.
    int count = 0;
    int total = 0;
    for (int k=region.kmin; k <= region.kmin + region.nk; ++k) {
        for (int j=region.jmin; j <= region.jmin + region.nj; ++j) {
            for (int i=region.imin; i <= region.imin + region.ni; ++i) {
                if (data[k][j][i] < 0)    count++;
                else                      count--;
                total++;
            }
        }
    }

    // Populate the corners of the ASDF by looking up values in
    // the array of results.
    for (int n=0; n < 8; ++n) {
        int i = region.imin + ((n & 4) ? region.ni : 0),
            j = region.jmin + ((n & 2) ? region.nj : 0),
            k = region.kmin + ((n & 1) ? region.nk : 0);
        asdf->d[n] = data[k][j][i];
    }


    if (count == total)             asdf->state = FILLED;
    else if (count == -total)       asdf->state = EMPTY;
    else if (region.voxels == 1)    asdf->state = LEAF;
    else {
        // Split and recurse
        Region octants[8];
        uint8_t bits = octsect(region, octants);
        for (int i=0; i < 8; ++i) {
            if (bits & (1<<i)) {
                asdf->branches[i] = build_from_vol(
                    data, octants[i], merge_leafs
                );
            }
        }

        // Pull d[i] values from children nodes
        get_d_from_children(asdf);

        // Merge cells if possible
        simplify(asdf, merge_leafs);
    }

    return asdf;
}





// From a floating-point pixel lattice, import stuff.
ASDF* import_lattice(float const*const*const distances,
                     const int ni, const int nj, const float offset,
                     const float mm_per_pixel, const _Bool merge_leafs)
{

    Region r = (Region){.imin = 0,  .jmin = 0,  .kmin = 0,
                        .ni = ni-1, .nj = nj-1, .nk = 1,
                        .voxels = (ni-1)*(nj-1)};

    float dx = mm_per_pixel*ni,
          dy = mm_per_pixel*nj;

    build_arrays(&r, 0, 0, 0, dx, dy, 0);
    ASDF* asdf = build_from_lattice(distances, r, offset, merge_leafs);
    free_arrays(&r);

    return asdf;
}


_STATIC_
ASDF* build_from_lattice(float const*const*const data,
                                const Region region, const float offset,
                                const _Bool merge_leafs)
{
    ASDF* asdf = calloc(1, sizeof(ASDF));

    if (region.voxels == 1) {
        // Pull x, y, and z values from the region
        *asdf = (ASDF) {
            .X = (Interval){region.X[0], region.X[region.ni]},
            .Y = (Interval){region.Y[0], region.Y[region.nj]},
            .Z = (Interval){region.Z[0], region.Z[region.nk]}
        };

        int count = 0;
        for (int n=0; n < 8; ++n) {
            int i = region.imin + ((n & 4) ? region.ni : 0),
                j = region.jmin + ((n & 2) ? region.nj : 0);
            asdf->d[n] = data[j][i] - offset;

            if (asdf->d[n] < 0) count++;
            else                count--;
        }

        if (count == 8)         asdf->state = FILLED;
        else if (count == -8)   asdf->state = EMPTY;
        else                    asdf->state = LEAF;

    } else {
        asdf->state = BRANCH;

        Region octants[8];
        uint8_t bits = octsect(region, octants);
        for (int i=0; i < 8; ++i) {
            if (bits & (1<<i)) {
                asdf->branches[i] = build_from_lattice(data, octants[i],
                                                       offset, merge_leafs);
            }
        }

        // Pull d[i] and X, Y, Z values from children nodes
        get_d_from_children(asdf);
        get_xyz_from_children(asdf);

        // Merge cells if possible
        simplify(asdf, merge_leafs);
    }

    return asdf;
}
