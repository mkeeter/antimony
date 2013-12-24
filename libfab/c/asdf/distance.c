#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "asdf/asdf.h"
#include "cam/distance.h"

#include "util/region.h"



/*  make_edt_column
 *
 *  Performs the first state of the Meijster distance transform
 *  on a single column.
 *
 *  Arguments:
 *      asdf    The asdf we're transforming
 *      nj      Number of positions
 *      empty   Value to indicate an empty column
 *      x       Single x position
 *      Y       Array of Y positions
 *      z       Single z position
 *      c       Output column
 */
 _STATIC_
void _make_edt_column(
    const _Bool*const img, const int nj,
    const int empty, int* const col)
{
    col[0] = img[0] ? 0 : empty;

    // Sweep up along the column
    for (int j=1; j <= nj; ++j) {
        col[j] = img[j] ? 0 : col[j-1] + 1;
    }

    // Sweep down the column
    for (int j=nj-1; j >= 0; --j) {
        col[j] = col[j+1] < col[j] ? col[j+1] + 1 : col[j];
    }
}


_STATIC_
void make_edt_column(
    const ASDF* const asdf, const int nj, const int empty,
    const float x, const float*const Y, const float z,
    int* const col)
{

    float samples[nj+1];
    for (int j=0; j <= nj; ++j) {
        samples[j] = asdf_sample(asdf, (Vec3f){x, Y[j], z});
    }

    _Bool img[nj+1];
    for (int j=0; j <= nj; ++j) {
        img[j] = samples[j] < 0;
    }
    int pos[nj+1];
    _make_edt_column(img, nj, empty, pos);


    for (int j=0; j <= nj; ++j) {
        img[j] = samples[j] >= 0;
    }
    int neg[nj+1];
    _make_edt_column(img, nj, empty, neg);

    for (int j=0; j <= nj; ++j) {
        neg[j] -= 1;
        col[j] = neg[j] > pos[j] ? -neg[j] : pos[j];
    }

}

////////////////////////////////////////////////////////////////////////////////

/*  make_edt_row
 *
 *  Performs the second state of the Meijster distance transform
 *  on a single row, calculating both positive and negative results.
 *
 *  Arguments:
 *      g       The g asdf
 *      ni      Number of positions minus 1
 *      pixels_per_mm   exactly what it sounds like
 *      X       Array of X positions
 *      y       Single y position
 *      z       Single z position
 *      row     Output row
 */
 _STATIC_
void _make_edt_row(
    const int*const g, const int ni, const float pixels_per_mm,
    float* const row)
{

    // Starting points of each region
    unsigned t[ni+1];
    unsigned s[ni+1];

    int q = 0;
    t[0] = 0;
    s[0] = 0;

    for (int u=1; u <= ni; ++u) {
        // Slide q backwards until we find a point where the
        // curve to be added is above the previous curve.
        while (q >= 0 && f_edt(t[q], s[q], g) > f_edt(t[q], u, g))
        {
            q--;
        }

        // If the new segment is below all previous curves,
        // then replace them all
        if (q < 0) {
            q = 0;
            s[0] = u;
        }

        // Otherwise, find the starting point for the new segment and
        // save it if it's within the bounds of the image.
        else {
            unsigned w = 1 + sep_edt(s[q], u, g);
            if (w <= ni) {
                q++;
                s[q] = u;
                t[q] = w;
            }
        }
    }

    // Finally, calculate and store distance values.
    for (int u = ni; u >= 0; --u) {
        row[u] =  sqrt(f_edt(u, s[q], g)) / pixels_per_mm;
        if (u == t[q]) q--;
    }
}

_STATIC_
void make_edt_row(
    const ASDF* const g, const int ni, const float pixels_per_mm,
    const float*const X, const float y, const float z,
    float* const row)
{

    // Sample a single row of the ASDF at the desired resolution
    float samples[ni+1];
    for (int i=0; i <= ni; ++i) {
        samples[i] = asdf_sample(g, (Vec3f){X[i], y, z});
    }


    int g_[ni+1];
    for (int i=0; i <= ni; ++i) {
        g_[i] = samples[i] < 0 ? 0 : round(samples[i]);
    }
    float pos[ni+1];
    _make_edt_row(g_, ni, pixels_per_mm, pos);


    for (int i=0; i <= ni; ++i) {
        g_[i] = samples[i] > 0 ? 0 : round(-samples[i]);
    }
    float neg[ni+1];
    _make_edt_row(g_, ni, pixels_per_mm, neg);

    for (int i=0; i <= ni; ++i) {
        row[i] = neg[i] > pos[i] ? -neg[i] : pos[i];
    }

}


////////////////////////////////////////////////////////////////////////////////

_STATIC_
_Bool make_D_asdf(
    ASDF** const D, const Region r, const int jmin, const float offset,
    const float* const bottom, const float* const top)
{

    if (*D == NULL) {
        *D = malloc(sizeof(ASDF));
        **D = (ASDF){
            .state = BRANCH,
            .X = (Interval){r.X[0], r.X[r.ni]},
            .Y = (Interval){r.Y[0], r.Y[r.nj]},
            .Z = (Interval){r.Z[0], r.Z[r.nk]},
            .branches = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL},
            .d = {0,0,0,0,0,0,0,0},
            .data = {.vp = NULL}
        };
    }


    if (r.ni == 1 && r.nj == 1) {

        (**D).d[0] = bottom[r.imin];
        (**D).d[2] = top[r.imin];
        (**D).d[4] = bottom[r.imin+1];
        (**D).d[6] = top[r.imin+1];

        // Copy across the Z plane (since this is a flat asdf)
        for (int a=0; a < 8; a += 2)    (**D).d[a+1] = (**D).d[a];

        // Subtract the offset and figure out cell type
        int e=0;
        for (int a=0; a < 8; ++a) {
            (**D).d[a] -= offset;
            if ((**D).d[a] >= 0)    e++;
            else                    e--;
        }
        if (e == 8)         (**D).state = EMPTY;
        else if (e == -8)   (**D).state = FILLED;
        else                (**D).state = LEAF;

        return true;

    } else {

        Region octants[8];
        uint8_t bits = octsect(r, octants);

        // See explanation below of the keystone cell
        uint8_t keystone = ((bits & (1 << 4)) ? 4 : 0) |
                           ((bits & (1 << 2)) ? 2 : 0);

        _Bool complete = false;

        for (int a=0; a < 8; ++a) {
            if ((bits & (1 << a)) &&
                jmin >= octants[a].jmin &&
                jmin < octants[a].jmin + octants[a].nj)
            {
                if (make_D_asdf(
                        &((**D).branches[a]), octants[a],
                        jmin, offset, bottom, top
                    ) && a == keystone)
                {
                    complete = true;
                }
            }
        }
        get_d_from_children(*D);

        if ( fabs((**D).X.lower - (-1.5)) < 1e-3 &&
            fabs((**D).X.upper - (-1.44)) < 1e-3 &&
            fabs((**D).Y.lower - (-0.5)) < 1e-3 &&
            fabs((**D).Y.upper - (-0.44)) < 1e-3) {
            printf("%g %g %g %g\n", (**D).X.lower, (**D).X.upper, (**D).Y.lower, (**D).Y.upper);
        }


        if (complete)   simplify(*D, true);


        return complete;
    }
}



_STATIC_
_Bool make_g_asdf(
    ASDF** const g, const Region r, const int imin, const float empty,
    const int* const left, const int* const right)
{

    if (*g == NULL) {
        *g = malloc(sizeof(ASDF));
        **g = (ASDF){
            .state = BRANCH,
            .X = (Interval){r.X[0], r.X[r.ni]},
            .Y = (Interval){r.Y[0], r.Y[r.nj]},
            .Z = (Interval){r.Z[0], r.Z[r.nk]},
            .branches = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL},
            .d = {0,0,0,0,0,0,0,0},
            .data = {.vp = NULL}
        };
    }


    if (r.ni == 1 && r.nj == 1) {
        (**g).state = LEAF;
        (**g).d[0] = left[r.jmin];
        (**g).d[2] = left[r.jmin+1];
        (**g).d[4] = right[r.jmin];
        (**g).d[6] = right[r.jmin+1];
        for (int a=0; a < 8; a += 2)    (**g).d[a+1] = (**g).d[a];

        int e = 0;
        for (int a=0; a < 8; ++a) {
            if ((**g).d[a] <= 0)            e--;
            else if ((**g).d[a] >= empty)   e++;
        }
        if (e == -8)        (**g).state = FILLED;
        else if (e == 8)    (**g).state = EMPTY;

        // This leaf is a complete, mergeable unit.
        return true;

    } else {

        Region octants[8];
        uint8_t bits = octsect(r, octants);

        /* The keystone cell is the last one that is visited
           in an octree subdivision.

           |---------|      |---------|     |----|
           |    | K  |      |    | K  |     | K  |
           |---------|      |---------|     |----|
           |    |    |                      |    |
           |---------|                      |----|

           If the keystone cell is complete, then we can
           simplify the entire branch.
        */

        uint8_t keystone = ((bits & (1 << 4)) ? 4 : 0) |
                           ((bits & (1 << 2)) ? 2 : 0);

        _Bool complete = false;


        for (int a=0; a < 8; ++a) {
            if ((bits & (1 << a)) &&
                imin >= octants[a].imin &&
                imin < octants[a].imin + octants[a].ni)
            {
                if (make_g_asdf(
                        &((**g).branches[a]), octants[a],
                        imin, empty, left, right
                    ) && a == keystone)
                {
                    complete = true;
                }
            }
        }
        get_d_from_children(*g);
        if (complete)   simplify(*g, true);

        return complete;
    }
}


_STATIC_
ASDF* asdf_edt1(const ASDF* const asdf, float offset, const Region r)
{
    int a[r.nj+1];
    int b[r.nj+1];

    const float pixels_per_mm = r.ni > r.nj ?
        r.ni /(r.X[r.ni] - r.X[0]) :
        r.nj / (r.Y[r.nj] - r.Y[0]);
    const int empty = pixels_per_mm * offset * sqrt(2);

    ASDF* g = NULL;
    make_edt_column(asdf, r.nj, empty, r.X[0], r.Y, r.Z[0], a);

    for (int i=0; i < r.ni; ++i) {

        printf("\rG:  %i/%i        ", i+1, r.ni);
        fflush(stdout);

        make_edt_column(
            asdf, r.nj, empty, r.X[i+1], r.Y, r.Z[0], (i % 2) ? a : b
        );

        const int* const left  = (i % 2) ? b : a;
        const int* const right = (i % 2) ? a : b;

        make_g_asdf(&g, r, i, empty, left, right);
    }

    simplify(g, true);

    return g;
}


_STATIC_
ASDF* asdf_edt2(const ASDF* const g, const float offset, const Region r)
{
    const float pixels_per_mm = r.ni > r.nj ?
        r.ni /(r.X[r.ni] - r.X[0]) :
        r.nj / (r.Y[r.nj] - r.Y[0]);

    float a[r.ni+1];
    float b[r.ni+1];

    ASDF* D = NULL;
    make_edt_row(g, r.ni, pixels_per_mm, r.X, r.Y[0], r.Z[0], a);

    for (int j=0; j < r.nj; ++j) {

        printf("\rD:  %i/%i        ", j+1, r.nj);
        fflush(stdout);

        make_edt_row(
            g, r.ni, pixels_per_mm, r.X, r.Y[j+1], r.Z[0], (j % 2) ? a : b
        );

        const float* const bottom  = (j % 2) ? b : a;
        const float* const top     = (j % 2) ? a : b;

        make_D_asdf(&D, r, j, offset, bottom, top);
    }

    simplify(D, true);

    return D;
}



ASDF* asdf_offset(
    const ASDF* const asdf, float offset, float pixels_per_mm)
{
    int ni = pixels_per_mm*(asdf->X.upper - asdf->X.lower),
        nj = pixels_per_mm*(asdf->Y.upper - asdf->Y.lower),
        nk = pixels_per_mm*(asdf->Z.upper - asdf->Z.lower);

    if (ni < 1) ni = 1;
    if (nj < 1) nj = 1;
    if (nk < 1) nk = 1;

    Region r = (Region){
        .imin=0, .jmin=0, .kmin=0,
        .ni=ni,  .nj=nj,  .nk=nk,
        .voxels=ni*nj*nk
    };
    build_arrays(&r,
        asdf->X.lower, asdf->Y.lower, asdf->Z.lower,
        asdf->X.upper, asdf->Y.upper, asdf->Z.upper
    );

    // Part 1: Generate the g asdf
    ASDF* const g = asdf_edt1(asdf, offset, r);

//    return g;

    // Part 2:  Generate the D asdf
    ASDF* const D = asdf_edt2(g, offset, r);
    printf("\r                \r");
    fflush(stdout);

    free_asdf(g);
    free_arrays(&r);

    return D;
}
