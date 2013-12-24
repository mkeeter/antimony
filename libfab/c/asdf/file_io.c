#include <stdlib.h>
#include <math.h>

#include "asdf/asdf.h"
#include "asdf/cache.h"
#include "asdf/file_io.h"

#include "util/region.h"
#include "util/macros.h"

/* Forward declarations */
_STATIC_
ASDF*  asdf_read_2_0(FILE* file);

_STATIC_
ASDF* _asdf_read_2_0(
    FILE* file, Region r, Corner* const cache, const float scale);

_STATIC_
ASDF*  asdf_read_1_4(FILE* file);

_STATIC_
ASDF* _asdf_read_1_4(FILE* file, Region r, Corner* cache);

_STATIC_
ASDF*  asdf_read_1_3(FILE* file);

_STATIC_
ASDF* _asdf_read_1_3(FILE* file, Region r, Corner* cache);

_STATIC_
ASDF*  asdf_read_1_2(FILE* file);

_STATIC_
ASDF* _asdf_read_1_2(FILE* file, Region r);

_STATIC_
ASDF*  asdf_read_1_1(FILE* file);

_STATIC_
ASDF* _asdf_read_1_1(FILE* file, Region r);

_STATIC_
ASDF*  asdf_read_1_0(FILE* file);

_STATIC_
void asdf_write_2_0(ASDF* const asdf, FILE* file);

_STATIC_
void _asdf_write_2_0(
    ASDF* const asdf, FILE* file, Region const r,
    Corner* const cache, const float scale);

_STATIC_
void  asdf_write_1_4(const ASDF* const asdf, FILE* file);

_STATIC_
void _asdf_write_1_4(const ASDF* const asdf, FILE* file);

_STATIC_
void  asdf_write_1_3(const ASDF* const asdf, FILE* file);

_STATIC_
void _asdf_write_1_3(const ASDF* const asdf, FILE* file);

_STATIC_
void  asdf_write_1_2(const ASDF* const asdf, FILE* file);

_STATIC_
void _asdf_write_1_2(const ASDF* const asdf, FILE* file);

_STATIC_
void  asdf_write_1_1(const ASDF* const asdf, FILE* file);

_STATIC_
void _asdf_write_1_1(const ASDF* const asdf, FILE* file);

_STATIC_
void  asdf_write_1_0(const ASDF* const asdf, FILE* file);

_STATIC_
void _asdf_write_1_0(const ASDF* const asdf, FILE* file);

/* End of forward declarations */


void asdf_write(ASDF* const asdf, const char* filename)
{
    FILE* file = fopen(filename, "wb");

    const int version_major = 2;
    const int version_minor = 0;

    if (version_major == 1 && version_minor == 0) {
        asdf_write_1_0(asdf, file);
    } else if (version_major == 1 && version_minor == 1) {
        asdf_write_1_1(asdf, file);
    } else if (version_major == 1 && version_minor == 2) {
        asdf_write_1_2(asdf, file);
    } else if (version_major == 1 && version_minor == 3) {
        asdf_write_1_3(asdf, file);
    } else if (version_major == 1 && version_minor == 4) {
        asdf_write_1_4(asdf, file);
    } else if (version_major == 2 && version_minor == 0) {
        asdf_write_2_0(asdf, file);
    } else {
        printf("Error: Invalid version number for .asdf file\n");
    }

    fclose(file);
}

ASDF* asdf_read(const char* filename)
{
    FILE* file = fopen(filename, "rb");
    char a = fgetc(file);
    char s = fgetc(file);
    char d = fgetc(file);
    char f = fgetc(file);
    if (a != 'A' || s != 'S' || d != 'D' || f != 'F') {
        printf("Error: did not recognize .asdf file\n");
        fclose(file);
        return NULL;
    }

    int version_major = fgetc(file);
    int version_minor = fgetc(file);

    ASDF* asdf = NULL;
    if (version_major == 1 && version_minor == 0) {
         asdf = asdf_read_1_0(file);
    } else if (version_major == 1 && version_minor == 1) {
        asdf = asdf_read_1_1(file);
    } else if (version_major == 1 && version_minor == 2) {
        asdf = asdf_read_1_2(file);
    } else if (version_major == 1 && version_minor == 3) {
        asdf = asdf_read_1_3(file);
    } else if (version_major == 1 && version_minor == 4) {
        asdf = asdf_read_1_4(file);
    } else if (version_major == 2 && version_minor == 0) {
        asdf = asdf_read_2_0(file);
    } else {
        printf("Error: Invalid version number for .asdf file\n");
    }
    fclose(file);
    return asdf;
}

////////////////////////////////////////////////////////////////////////////////

_STATIC_
void asdf_write_2_0(ASDF* const asdf, FILE* file)
{
    fprintf(file, "ASDF%c%c", 2, 0);

    int ni, nj, nk;
    find_dimensions(asdf, &ni, &nj, &nk);

    float header_f[6] = {
        asdf->X.lower, asdf->X.upper,
        asdf->Y.lower, asdf->Y.upper,
        asdf->Z.lower, asdf->Z.upper
    };
    int32_t header_i[3] = {ni, nj, nk};

    for (int i=0; i < sizeof(header_f); ++i) {
        fputc(((char*)&header_f)[i], file);
    }
    for (int i=0; i < sizeof(header_i); ++i) {
        fputc(((char*)&header_i)[i], file);
    }

    // We'll get fifteen bits of resolution on each side
    const float min = asdf_get_min(asdf);
    const float max = asdf_get_max(asdf);

    float scale = 32767;
    if (fabs(max) > fabs(min))  scale /= fabs(max);
    else                        scale /= fabs(min);

    // Write the scale to file
    for (int i=0; i < sizeof(scale); ++i) {
        fputc( ((char*)&scale)[i], file);
    }

    Region r = (Region){
        .imin = 0,  .jmin = 0,  .kmin = 0,
        .ni   = ni, .nj   = nj, .nk   = nk,
        .voxels = ni*nj*nk
    };
    Corner* const cache = calloc(1, sizeof(Corner));
    cache->value = NAN;

    _asdf_write_2_0(asdf, file, r, cache, scale);

    free_corner_cache(cache);
}

_STATIC_
void _asdf_write_2_0(
    ASDF* const asdf, FILE* file, Region const r,
    Corner* const cache, const float scale)
{
    if (asdf->state == BRANCH) {
        fprintf(file, "B");

        // Write out the populated branches as a bit field
        char branching = 0;
        for (int i=0; i < 8; ++i) {
            if (asdf->branches[i]) {
                branching |= (1 << i);
            }
        }
        fputc(branching, file);

        Region octants[8];
        octsect_merged(r, asdf, octants);

        // Get a subcache to speed up lookups
        Corner* subcache = corner_subcache(
            cache,
            r.imin, r.imin+r.ni,
            r.jmin, r.jmin+r.nj,
            r.kmin, r.kmin+r.nk
        );

        // Write out the branches one by one
        for (int i=0; i < 8; ++i) {
            if (asdf->branches[i]) {
                _asdf_write_2_0(
                    asdf->branches[i], file, octants[i],
                    subcache, scale
                );
            }
        }
    } else {
        if (asdf->state == FILLED)      fprintf(file, "F");
        else if (asdf->state == EMPTY)  fprintf(file, "E");
        else if (asdf->state == LEAF)   fprintf(file, "L");

        for (int a=0; a < 8; ++a) {
            Corner* const pt = get_corner(
                cache,
                r.imin + (a & 4 ? r.ni : 0),
                r.jmin + (a & 2 ? r.nj : 0),
                r.kmin + (a & 1 ? r.nk : 0)
            );
            if (isnan(pt->value)) {
                pt->value = asdf->d[a];
                int16_t v = BOUND(pt->value * scale, -32768, 32767);
                // Preserve corner sign to ensure closed shapes
                if (v == 0 && pt->value < 0) {
                    v = -1;
                }
                for (int b=0; b < sizeof(v); ++b) {
                    fputc( ((char*)&v)[b], file);
                }
            }
        }
    }
}

_STATIC_
ASDF*  asdf_read_2_0(FILE* file)
{
    float xmin, xmax, ymin, ymax, zmin, zmax;
    int32_t ni, nj, nk;
    float scale;

    fscanf(file, "%4c", (char*)&xmin);
    fscanf(file, "%4c", (char*)&xmax);
    fscanf(file, "%4c", (char*)&ymin);
    fscanf(file, "%4c", (char*)&ymax);
    fscanf(file, "%4c", (char*)&zmin);
    fscanf(file, "%4c", (char*)&zmax);

    fscanf(file, "%4c", (char*)&ni);
    fscanf(file, "%4c", (char*)&nj);
    fscanf(file, "%4c", (char*)&nk);

    fscanf(file, "%4c", (char*)&scale);

    Region r = (Region){
        .imin=0, .jmin=0, .kmin=0,
        .ni=ni,  .nj=nj,  .nk=nk,
        .voxels = ni*nj*nk
    };
    build_arrays(&r, xmin, ymin, zmin, xmax, ymax, zmax);

    Corner* const cache = calloc(1, sizeof(Corner));
    cache->value = NAN;

    ASDF* asdf = _asdf_read_2_0(file, r, cache, scale);

    free_arrays(&r);
    free_corner_cache(cache);

    return asdf;
}

_STATIC_
ASDF* _asdf_read_2_0(
    FILE* file, Region r, Corner* const cache, const float scale)
{
    ASDF* const asdf = calloc(1, sizeof(ASDF));
    *asdf = (ASDF) {
        .X=(Interval){.lower=r.X[0], .upper=r.X[r.ni]},
        .Y=(Interval){.lower=r.Y[0], .upper=r.Y[r.nj]},
        .Z=(Interval){.lower=r.Z[0], .upper=r.Z[r.nk]},
    };

    const char c = fgetc(file);
    if (c == 'B') {
        asdf->state = BRANCH;

        // Get the bitfield marking split pattern
        const uint8_t branching = fgetc(file);

        // Put a dummy pointer in the appropriate places in asdf->branches
        // so that octsect_merged splits in the right way.
        for (int i=0; i < 8; ++i) {
            if (branching & (1 << i)) {
                asdf->branches[i] = (ASDF*)0x1;
            }
        }

        // Split the region
        Region octants[8];
        octsect_merged(r, asdf, octants);

        // Get a subcache to speed up lookups
        Corner* subcache = corner_subcache(
            cache,
            r.imin, r.imin+r.ni,
            r.jmin, r.jmin+r.nj,
            r.kmin, r.kmin+r.nk
        );

        // Read in subtrees if they exist
        for (int i=0; i < 8; ++i) {
            if (asdf->branches[i]) {
                asdf->branches[i] = _asdf_read_2_0(
                    file, octants[i], subcache, scale
                );
            }
        }

        // Pull corner values and positions from children
        get_d_from_children(asdf);

    } else {

        if (c == 'F')       asdf->state = FILLED;
        else if (c == 'E')  asdf->state = EMPTY;
        else if (c == 'L')  asdf->state = LEAF;

        // Pull values from the cache or from the file
        for (int a=0; a<8; ++a) {

            // Get a cache pointer to this corner
            Corner* const pt = get_corner(
                cache,
                r.imin + (a & 4 ? r.ni : 0),
                r.jmin + (a & 2 ? r.nj : 0),
                r.kmin + (a & 1 ? r.nk : 0)
            );

            // If this cache pointer hasn't already been populated, then
            // we'll read in the value from the file.
            if (isnan(pt->value)) {
                int16_t v;
                fscanf(file, "%2c", (char*)&v);
                pt->value = v / scale;
                asdf->d[a] = pt->value;
            } else {
                asdf->d[a] = pt->value;
            }
        }
    }

    return asdf;
}


////////////////////////////////////////////////////////////////////////////////

_STATIC_
void asdf_write_1_4(const ASDF* const asdf, FILE* file)
{
    fprintf(file, "ASDF%c%c", 1, 4);

    int ni, nj, nk;
    find_dimensions(asdf, &ni, &nj, &nk);

    float header_f[6] = {
        asdf->X.lower, asdf->X.upper,
        asdf->Y.lower, asdf->Y.upper,
        asdf->Z.lower, asdf->Z.upper
    };
    int32_t header_i[3] = {ni, nj, nk};

    for (int i=0; i < sizeof(header_f); ++i) {
        fputc(((char*)&header_f)[i], file);
    }
    for (int i=0; i < sizeof(header_i); ++i) {
        fputc(((char*)&header_i)[i], file);
    }

    Corner* cache = fill_corner_cache_all(asdf);
    write_cache(cache, file);
    free_corner_cache(cache);

    _asdf_write_1_4(asdf, file);
}


_STATIC_
void _asdf_write_1_4(const ASDF* const asdf, FILE* file)
{
    if (!asdf)  {
        return;
    } else if (asdf->state == BRANCH) {
        fprintf(file, "B");

        // Write out the populated branches as a bit field

        char branching = 0;
        for (int i=0; i < 8; ++i) {
            if (asdf->branches[i]) {
                branching |= (1 << i);
            }
        }
        fputc(branching, file);

        // Write out the branches one by one
        for (int i=0; i < 8; ++i) {
            _asdf_write_1_4(asdf->branches[i], file);
        }

    } else if (asdf->state == FILLED) {
        fprintf(file, "F");
    } else if (asdf->state == EMPTY) {
        fprintf(file, "E");
    } else if (asdf->state == LEAF) {
        fprintf(file, "L");
    }
}


_STATIC_
ASDF*  asdf_read_1_4(FILE* file)
{
    float xmin, xmax, ymin, ymax, zmin, zmax;
    int32_t ni, nj, nk;

    fscanf(file, "%4c", ((char*)&xmin));
    fscanf(file, "%4c", ((char*)&xmax));
    fscanf(file, "%4c", ((char*)&ymin));
    fscanf(file, "%4c", ((char*)&ymax));
    fscanf(file, "%4c", ((char*)&zmin));
    fscanf(file, "%4c", ((char*)&zmax));

    fscanf(file, "%4c", ((char*)&ni));
    fscanf(file, "%4c", ((char*)&nj));
    fscanf(file, "%4c", ((char*)&nk));

    Region r = (Region){
        .imin=0, .jmin=0, .kmin=0,
        .ni=ni,  .nj=nj,  .nk=nk,
        .voxels = ni*nj*nk
    };
    build_arrays(&r, xmin, ymin, zmin, xmax, ymax, zmax);

    Corner* cache = read_cache(file);
    ASDF* asdf = _asdf_read_1_4(file, r, cache);

    free_arrays(&r);
    free_corner_cache(cache);

    return asdf;
}


_STATIC_
ASDF* _asdf_read_1_4(FILE* file, Region r, Corner* cache)
{
    ASDF* const asdf = calloc(1, sizeof(ASDF));
    *asdf = (ASDF) {
        .X=(Interval){.lower=r.X[0], .upper=r.X[r.ni]},
        .Y=(Interval){.lower=r.Y[0], .upper=r.Y[r.nj]},
        .Z=(Interval){.lower=r.Z[0], .upper=r.Z[r.nk]},
    };

    const char c = fgetc(file);
    if (c == 'B') {
        asdf->state = BRANCH;

        // Get a subcache to speed up lookups
        Corner* subcache = corner_subcache(cache,
                                           r.imin, r.imin+r.ni,
                                           r.jmin, r.jmin+r.nj,
                                           r.kmin, r.kmin+r.nk);

        // Get the bitfield marking split pattern
        uint8_t branching = fgetc(file);

        // Put a dummy pointer in the appropriate places in asdf->branches
        // so that octsect_merged splits in the right way.
        for (int i=0; i < 8; ++i) {
            if (branching & (1 << i)) {
                asdf->branches[i] = (ASDF*)0x1;
            }
        }

        // Split the region
        Region octants[8];
        octsect_merged(r, asdf, octants);

        // Read in subtrees if they exist
        for (int i=0; i < 8; ++i) {
            if (branching & (1 << i)) {
                asdf->branches[i] = _asdf_read_1_4(file, octants[i],
                                                   subcache);
            }
        }

        // Pull corner values and positions from children
        get_d_from_children(asdf);
        get_xyz_from_children(asdf);

        return asdf;
    } else {

        if (c == 'F')       asdf->state = FILLED;
        else if (c == 'E')  asdf->state = EMPTY;
        else if (c == 'L')  asdf->state = LEAF;

        // Pull values from the cache!
        for (int a=0; a<8; ++a) {
            uint16_t i = (a & 4) ? r.imin + r.ni : r.imin;
            uint16_t j = (a & 2) ? r.jmin + r.nj : r.jmin;
            uint16_t k = (a & 1) ? r.kmin + r.nk : r.kmin;
            Corner* corner = get_corner(cache, i, j, k);
            asdf->d[a] = corner->value;
        }
    }

    return asdf;
}

////////////////////////////////////////////////////////////////////////////////

_STATIC_
void asdf_write_1_3(const ASDF* const asdf, FILE* file)
{
    fprintf(file, "ASDF%c%c", 1, 3);

    int ni, nj, nk;
    find_dimensions(asdf, &ni, &nj, &nk);

    float header_f[6] = {asdf->X.lower, asdf->X.upper,
                         asdf->Y.lower, asdf->Y.upper,
                         asdf->Z.lower, asdf->Z.upper};
    int32_t header_i[3] = {ni, nj, nk};

    for (int i=0; i < 4*6; ++i) {
        fputc(((char*)&header_f)[i], file);
    }
    for (int i=0; i < 4*3; ++i) {
        fputc(((char*)&header_i)[i], file);
    }

    Corner* cache = fill_corner_cache(asdf);
    write_cache(cache, file);
    free_corner_cache(cache);

    _asdf_write_1_3(asdf, file);
}


_STATIC_
void _asdf_write_1_3(const ASDF* const asdf, FILE* file)
{
    if (!asdf)  {
        return;
    } else if (asdf->state == BRANCH) {
        fprintf(file, "B");

        // Write out the populated branches as a bit field

        char branching = 0;
        for (int i=0; i < 8; ++i) {
            if (asdf->branches[i]) {
                branching |= (1 << i);
            }
        }
        fputc(branching, file);

        // Write out the branches one by one
        for (int i=0; i < 8; ++i) {
            _asdf_write_1_3(asdf->branches[i], file);
        }

    } else if (asdf->state == FILLED) {
        fprintf(file, "F");
    } else if (asdf->state == EMPTY) {
        fprintf(file, "E");
    } else if (asdf->state == LEAF) {
        fprintf(file, "L");
    }
}


_STATIC_
ASDF*  asdf_read_1_3(FILE* file)
{
    float xmin, xmax, ymin, ymax, zmin, zmax;
    int32_t ni, nj, nk;

    fscanf(file, "%4c", ((char*)&xmin));
    fscanf(file, "%4c", ((char*)&xmax));
    fscanf(file, "%4c", ((char*)&ymin));
    fscanf(file, "%4c", ((char*)&ymax));
    fscanf(file, "%4c", ((char*)&zmin));
    fscanf(file, "%4c", ((char*)&zmax));

    fscanf(file, "%4c", ((char*)&ni));
    fscanf(file, "%4c", ((char*)&nj));
    fscanf(file, "%4c", ((char*)&nk));


    Region r = (Region){.imin = 0,  .jmin = 0,  .kmin = 0,
                        .ni   = ni, .nj   = nj, .nk   = nk,
                        .voxels = ni*nj*nk
                        };
    build_arrays(&r, xmin, ymin, zmin, xmax, ymax, zmax);

    Corner* cache = read_cache(file);
    ASDF* asdf = _asdf_read_1_3(file, r, cache);

    free_arrays(&r);
    free_corner_cache(cache);

    return asdf;
}


_STATIC_
ASDF* _asdf_read_1_3(FILE* file, Region r, Corner* cache)
{
    ASDF* asdf = calloc(1, sizeof(ASDF));

    char c = fgetc(file);
    if (c == 'B') {
        asdf->state = BRANCH;

        // Get a subcache to speed up lookups
        Corner* subcache = corner_subcache(cache,
                                           r.imin, r.imin+r.ni,
                                           r.jmin, r.jmin+r.nj,
                                           r.kmin, r.kmin+r.nk);

        // Get the bitfield marking split pattern
        uint8_t branching = fgetc(file);

        // Set pointers to 1 if the bitfield says there's a
        // subtree at this position.
        for (int i=0; i < 8; ++i) {
            if (branching & (1 << i)) {
                asdf->branches[i] = (ASDF*)0x1;
            }
        }

        // Split the region
        Region octants[8];
        octsect_merged(r, asdf, octants);

        // Read in subtrees if they exist
        for (int i=0; i < 8; ++i) {
            if (asdf->branches[i]) {
                asdf->branches[i] = _asdf_read_1_3(file, octants[i],
                                                   subcache);
            }
        }

        // Pull corner values and positions from children
        get_d_from_children(asdf);
        get_xyz_from_children(asdf);

        return asdf;
    } else if (c == 'F') {
        asdf->state = FILLED;
        for (int c=0; c<8; ++c) asdf->d[c] = -1;
    } else if (c == 'E') {
        asdf->state = EMPTY;
        for (int c=0; c<8; ++c) asdf->d[c] = 1;
    } else if (c == 'L') {
        asdf->state = LEAF;

        // Pull values from the cache!
        for (int a=0; a<8; ++a) {
            uint16_t i = (a & 4) ? r.imin + r.ni : r.imin;
            uint16_t j = (a & 2) ? r.jmin + r.nj : r.jmin;
            uint16_t k = (a & 1) ? r.kmin + r.nk : r.kmin;
            Corner* corner = get_corner(cache, i, j, k);
            asdf->d[a] = corner->value;
        }
    }

    // Set coordinates from the region.
    asdf->X.lower = r.X[0];
    asdf->X.upper = r.X[r.ni];
    asdf->Y.lower = r.Y[0];
    asdf->Y.upper = r.Y[r.nj];
    asdf->Z.lower = r.Z[0];
    asdf->Z.upper = r.Z[r.nk];

    return asdf;
}

////////////////////////////////////////////////////////////////////////////////

_STATIC_
void asdf_write_1_2(const ASDF* const asdf, FILE* file)
{
    fprintf(file, "ASDF%c%c", 1, 2);

    int ni, nj, nk;
    find_dimensions(asdf, &ni, &nj, &nk);

    float header_f[6] = {asdf->X.lower, asdf->X.upper,
                         asdf->Y.lower, asdf->Y.upper,
                         asdf->Z.lower, asdf->Z.upper};
    int32_t header_i[3] = {ni, nj, nk};

    for (int i=0; i < 4*6; ++i) {
        fputc(((char*)&header_f)[i], file);
    }
    for (int i=0; i < 4*3; ++i) {
        fputc(((char*)&header_i)[i], file);
    }

    _asdf_write_1_2(asdf, file);
}


_STATIC_
void _asdf_write_1_2(const ASDF* const asdf, FILE* file)
{
    if (!asdf) {
        fprintf(file, " ");
        return;
    }
    else if (asdf->state == BRANCH) {
        fprintf(file, "B");
        for (int i=0; i < 8; ++i)
            _asdf_write_1_2(asdf->branches[i], file);
        return;
    } else if (asdf->state == FILLED) {
        fprintf(file, "F");
    } else if (asdf->state == EMPTY) {
        fprintf(file, "E");
    } else if (asdf->state == LEAF) {
        fprintf(file, "L");
        for (int i=0; i<4*8; ++i) {
            fputc(((char*)&asdf->d)[i], file);
        }
    }
}


_STATIC_
ASDF*  asdf_read_1_2(FILE* file)
{
    float xmin, xmax, ymin, ymax, zmin, zmax;
    int32_t ni, nj, nk;

    fscanf(file, "%4c", ((char*)&xmin));
    fscanf(file, "%4c", ((char*)&xmax));
    fscanf(file, "%4c", ((char*)&ymin));
    fscanf(file, "%4c", ((char*)&ymax));
    fscanf(file, "%4c", ((char*)&zmin));
    fscanf(file, "%4c", ((char*)&zmax));

    fscanf(file, "%4c", ((char*)&ni));
    fscanf(file, "%4c", ((char*)&nj));
    fscanf(file, "%4c", ((char*)&nk));

    Region r = (Region){.imin = 0,  .jmin = 0,  .kmin = 0,
                        .ni   = ni, .nj   = nj, .nk   = nk,
                        .voxels = ni*nj*nk
                        };
    build_arrays(&r, xmin, ymin, zmin, xmax, ymax, zmax);

    ASDF* asdf = _asdf_read_1_2(file, r);

    free_arrays(&r);

    return asdf;
}


_STATIC_
ASDF* _asdf_read_1_2(FILE* file, Region r)
{
    char c = fgetc(file);
    if (c == ' ')
        return NULL;

    ASDF* asdf = calloc(1, sizeof(ASDF));
    if (c == 'B') {
        asdf->state = BRANCH;

        Region octants[8];
        octsect(r, octants);

        /*
        Read in subtrees
        Some of the octants may be invalid, but that's okay, because
        there won't be a subtree in that slot so we'll return as soon
        as we see the empty character.
        */
        for (int i=0; i < 8; ++i) {
            asdf->branches[i] = _asdf_read_1_2(file, octants[i]);
        }

        /*
        Handle partial merges, where cells are merged along
        one axis but not all three.  This can be detected if
        a region could be split along an axis, but there aren't
        cells in both slots.
        */

        // X axis
        if (r.ni > 1 && !asdf->branches[4]) {
            for (int a=0; a < 8; ++a) {
                if (a & 4 || !asdf->branches[a]) continue;
                asdf->branches[a]->X.upper = r.X[r.ni];
            }
        }

        // Y axis
        if (r.nj > 1 && !asdf->branches[2]) {
            for (int a=0; a < 8; ++a) {
                if (a & 2 || !asdf->branches[a]) continue;
                asdf->branches[a]->Y.upper = r.Y[r.nj];
            }
        }

        // Z axis
        if (r.nk > 1 && !asdf->branches[1]) {
            for (int a=0; a < 8; ++a) {
                if (a & 1 || !asdf->branches[a]) continue;
                asdf->branches[a]->Z.upper = r.Z[r.nk];
            }
        }

        // Pull corner values and positions from children
        get_d_from_children(asdf);
        get_xyz_from_children(asdf);

        return asdf;
    } else if (c == 'F') {
        asdf->state = FILLED;
        for (int c=0; c<8; ++c) {
            asdf->d[c] = -1;
        }
    } else if (c == 'E') {
        asdf->state = EMPTY;
        for (int c=0; c<8; ++c) {
            asdf->d[c] = 1;
        }
    } else if (c == 'L') {
        asdf->state = LEAF;
        for (int c=0; c<8; ++c) {
            fscanf(file, "%4c", ((char*)&asdf->d[c]));
        }
    }

    // Set coordinates from the region.
    asdf->X.lower = r.X[0];
    asdf->X.upper = r.X[r.ni];
    asdf->Y.lower = r.Y[0];
    asdf->Y.upper = r.Y[r.nj];
    asdf->Z.lower = r.Z[0];
    asdf->Z.upper = r.Z[r.nk];

    return asdf;
}

////////////////////////////////////////////////////////////////////////////////

_STATIC_
void asdf_write_1_1(const ASDF* const asdf, FILE* file)
{
    fprintf(file, "ASDF%c%c", 1, 1);

    int ni, nj, nk;
    find_dimensions(asdf, &ni, &nj, &nk);

    float header_f[6] = {asdf->X.lower, asdf->X.upper,
                         asdf->Y.lower, asdf->Y.upper,
                         asdf->Z.lower, asdf->Z.upper};
    int32_t header_i[3] = {ni, nj, nk};

    for (int i=0; i < 4*6; ++i) {
        fprintf(file, "%c", ((char*)&header_f)[i]);
    }
    for (int i=0; i < 4*3; ++i) {
        fprintf(file, "%c", ((char*)&header_i)[i]);
    }

    _asdf_write_1_1(asdf, file);
}


_STATIC_
void _asdf_write_1_1(const ASDF* const asdf, FILE* file)
{
    if (!asdf) {
        fprintf(file, " ");
        return;
    }
    else if (asdf->state == BRANCH) {
        fprintf(file, "B");
        for (int i=0; i < 8; ++i)
            _asdf_write_1_1(asdf->branches[i], file);
        return;
    } else if (asdf->state == FILLED) {
        fprintf(file, "F");
    } else if (asdf->state == EMPTY) {
        fprintf(file, "E");
    } else if (asdf->state == LEAF) {
        fprintf(file, "L");
    }

    for (int i=0; i<4*8; ++i) {
        fprintf(file, "%c", ((char*)&asdf->d)[i] );
    }
}


_STATIC_
ASDF*  asdf_read_1_1(FILE* file)
{
    float xmin, xmax, ymin, ymax, zmin, zmax;
    int32_t ni, nj, nk;

    fscanf(file, "%4c", ((char*)&xmin));
    fscanf(file, "%4c", ((char*)&xmax));
    fscanf(file, "%4c", ((char*)&ymin));
    fscanf(file, "%4c", ((char*)&ymax));
    fscanf(file, "%4c", ((char*)&zmin));
    fscanf(file, "%4c", ((char*)&zmax));

    fscanf(file, "%4c", ((char*)&ni));
    fscanf(file, "%4c", ((char*)&nj));
    fscanf(file, "%4c", ((char*)&nk));

    Region r = (Region){.imin = 0,  .jmin = 0,  .kmin = 0,
                        .ni   = ni, .nj   = nj, .nk   = nk,
                        .voxels = ni*nj*nk
                        };
    build_arrays(&r, xmin, ymin, zmin, xmax, ymax, zmax);

    ASDF* asdf = _asdf_read_1_1(file, r);

    free_arrays(&r);

    return asdf;
}


_STATIC_
ASDF* _asdf_read_1_1(FILE* file, Region r)
{
    char c = fgetc(file);
    if (c == ' ')
        return NULL;

    ASDF* asdf = calloc(1, sizeof(ASDF));
    if (c == 'B') {
        asdf->state = BRANCH;

        Region octants[8];
        octsect(r, octants);

        /*
        Read in subtrees
        Some of the octants may be invalid, but that's okay, because
        there won't be a subtree in that slot so we'll return as soon
        as we see the empty character.
        */
        for (int i=0; i < 8; ++i) {
            asdf->branches[i] = _asdf_read_1_1(file, octants[i]);
        }

        /*
        Handle partial merges, where cells are merged along
        one axis but not all three.  This can be detected if
        a region could be split along an axis, but there aren't
        cells in both slots.
        */

        // X axis
        if (r.ni > 1 && !asdf->branches[4]) {
            for (int a=0; a < 8; ++a) {
                if (a & 4 || !asdf->branches[a]) continue;
                asdf->branches[a]->X.upper = r.X[r.ni];
            }
        }

        // Y axis
        if (r.nj > 1 && !asdf->branches[2]) {
            for (int a=0; a < 8; ++a) {
                if (a & 2 || !asdf->branches[a]) continue;
                asdf->branches[a]->Y.upper = r.Y[r.nj];
            }
        }

        // Z axis
        if (r.nk > 1 && !asdf->branches[1]) {
            for (int a=0; a < 8; ++a) {
                if (a & 1 || !asdf->branches[a]) continue;
                asdf->branches[a]->Z.upper = r.Z[r.nk];
            }
        }

        // Pull corner values and positions from children
        get_d_from_children(asdf);
        get_xyz_from_children(asdf);

        return asdf;
    } else if (c == 'F') {
        asdf->state = FILLED;
    } else if (c == 'E') {
        asdf->state = EMPTY;
    } else if (c == 'L') {
        asdf->state = LEAF;
    }

    for (int c=0; c<8; ++c) {
        fscanf(file, "%4c", ((char*)&asdf->d[c]));
    }

    // Set coordinates from the region.
    asdf->X.lower = r.X[0];
    asdf->X.upper = r.X[r.ni];
    asdf->Y.lower = r.Y[0];
    asdf->Y.upper = r.Y[r.nj];
    asdf->Z.lower = r.Z[0];
    asdf->Z.upper = r.Z[r.nk];

    return asdf;
}

////////////////////////////////////////////////////////////////////////////////


_STATIC_
void asdf_write_1_0(const ASDF* const asdf, FILE* file)
{
    fprintf(file, "ASDF%c%c", 1, 0);
    _asdf_write_1_0(asdf, file);
}


_STATIC_
void _asdf_write_1_0(const ASDF* const asdf, FILE* file)
{
    if (!asdf) {
        fprintf(file, " ");
        return;
    }
    else if (asdf->state == BRANCH) {
        fprintf(file, "B");
        for (int i=0; i < 8; ++i)
            _asdf_write_1_0(asdf->branches[i], file);
        return;
    } else if (asdf->state == FILLED) {
        fprintf(file, "F");
    } else if (asdf->state == EMPTY) {
        fprintf(file, "E");
    } else if (asdf->state == LEAF) {
        fprintf(file, "L");
    }

    // Copy a bunch of floats into a data array
    float data[14] = {asdf->X.lower, asdf->X.upper,
                      asdf->Y.lower, asdf->Y.upper,
                      asdf->Z.lower, asdf->Z.upper};
    for (int c=0; c < 8; ++c) data[c+6] = asdf->d[c];

    for (int i=0; i<4*14; ++i) {
        fprintf(file, "%c", ((char*)&data)[i] );
    }

}

_STATIC_
ASDF* asdf_read_1_0(FILE* file)
{
    char c = fgetc(file);
    if (c == ' ')
        return NULL;

    ASDF* asdf = calloc(1, sizeof(ASDF));
    if (c == 'B') {
        asdf->state = BRANCH;
        for (int i=0; i < 8; ++i)
            asdf->branches[i] = asdf_read_1_0(file);
        get_d_from_children(asdf);
        get_xyz_from_children(asdf);
        return asdf;
    } else if (c == 'F') {
        asdf->state = FILLED;
    } else if (c == 'E') {
        asdf->state = EMPTY;
    } else if (c == 'L') {
        asdf->state = LEAF;
    }

    fscanf(file, "%4c", ((char*)&asdf->X.lower));
    fscanf(file, "%4c", ((char*)&asdf->X.upper));
    fscanf(file, "%4c", ((char*)&asdf->Y.lower));
    fscanf(file, "%4c", ((char*)&asdf->Y.upper));
    fscanf(file, "%4c", ((char*)&asdf->Z.lower));
    fscanf(file, "%4c", ((char*)&asdf->Z.upper));
    for (int c=0; c<8; ++c)
        fscanf(file, "%4c", ((char*)&asdf->d[c]));

    return asdf;
}
