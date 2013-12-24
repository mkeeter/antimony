#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "asdf/asdf.h"

#include "tree/packed.h"
#include "tree/node/node.h"
#include "tree/node/opcodes.h"

#include "util/region.h"
#include "util/vec3f.h"

void build_arrays(Region* const R,
                  const float xmin, const float ymin, const float zmin,
                  const float xmax, const float ymax, const float zmax)
{
    R->X = malloc((R->ni+1)*sizeof(float));
    for (int i = 0; i <= R->ni; ++i)
        R->X[i] = xmin*(R->ni - i)/(float)R->ni + xmax*i/(float)R->ni;

    R->Y = malloc((R->nj+1)*sizeof(float));
    for (int j = 0; j <= R->nj; ++j)
        R->Y[j] = ymin*(R->nj - j)/(float)R->nj + ymax*j/(float)R->nj;

    R->Z = malloc((R->nk+1)*sizeof(float));
    for (int k = 0; k <= R->nk; ++k)
        R->Z[k] = zmin*(R->nk - k)/(float)R->nk + zmax*k/(float)R->nk;

    R->L = malloc((R->nk+1)*sizeof(uint16_t));
    for (int h = 0; h <= R->nk; ++h)
        R->L[h] = (zmax == zmin ) ? 65535 : (65535 * h) / (R->nk);
}

void free_arrays(Region* const R)
{
//    printf("Freeing arrays!\n");
    free(R->X);
    free(R->Y);
    free(R->Z);
    free(R->L);
    R->X = NULL;
    R->Y = NULL;
    R->Z = NULL;
    R->L = NULL;
}

////////////////////////////////////////////////////////////////////////////////

void bisect_x(const Region r, Region* const A, Region* const B)
{
    *A = (Region) {
        r.imin, r.jmin, r.kmin,
        r.ni/2, r.nj,   r.nk,
        (r.ni/2)*r.nj*r.nk,
        r.X ? r.X : NULL,
        r.Y ? r.Y : NULL,
        r.Z ? r.Z : NULL,
        r.L ? r.L : NULL};
    *B = (Region) {
        r.imin + r.ni/2, r.jmin, r.kmin,
        r.ni - r.ni/2, r.nj, r.nk,
        (r.ni - r.ni/2) * r.nj * r.nk,
        r.X ? r.X + r.ni/2 : NULL,
        r.Y ? r.Y : NULL,
        r.Z ? r.Z : NULL,
        r.L ? r.L : NULL};
}

////////////////////////////////////////////////////////////////////////////////

void bisect_y(const Region r, Region* const A, Region* const B)
{
    *A = (Region) {
        r.imin, r.jmin, r.kmin,
        r.ni,   r.nj/2, r.nk,
        r.ni*(r.nj/2)*r.nk,
        r.X ? r.X : NULL,
        r.Y ? r.Y : NULL,
        r.Z ? r.Z : NULL,
        r.L ? r.L : NULL};
    *B = (Region) {
        r.imin, r.jmin+r.nj/2, r.kmin,
        r.ni,   r.nj - r.nj/2, r.nk,
        r.ni*(r.nj - r.nj/2)*r.nk,
        r.X ? r.X : NULL,
        r.Y ? r.Y + r.nj/2: NULL,
        r.Z ? r.Z : NULL,
        r.L ? r.L : NULL};
}

////////////////////////////////////////////////////////////////////////////////

void bisect_z(const Region r, Region* const A, Region* const B)
{
    *A = (Region) {
        r.imin, r.jmin, r.kmin,
        r.ni,   r.nj,   r.nk/2,
        r.ni*r.nj*(r.nk/2),
        r.X ? r.X : NULL,
        r.Y ? r.Y : NULL,
        r.Z ? r.Z : NULL,
        r.L ? r.L : NULL};
    *B = (Region) {
        r.imin, r.jmin, r.kmin+r.nk/2,
        r.ni,   r.nj,   r.nk - r.nk/2,
        r.ni*r.nj*(r.nk - r.nk/2),
        r.X ? r.X : NULL,
        r.Y ? r.Y : NULL,
        r.Z ? r.Z + r.nk/2 : NULL,
        r.L ? r.L + r.nk/2 : NULL};
}

////////////////////////////////////////////////////////////////////////////////

void bisect_x_overlap(const Region r, Region* const A, Region* const B)
{
    const int o = r.ni/32;

    *A = (Region) {
        r.imin, r.jmin, r.kmin,
        r.ni/2+o, r.nj,   r.nk,
        (r.ni/2+o)*r.nj*r.nk,
        r.X ? r.X : NULL,
        r.Y ? r.Y : NULL,
        r.Z ? r.Z : NULL,
        r.L ? r.L : NULL};
    *B = (Region) {
        r.imin + r.ni/2 - o, r.jmin, r.kmin,
        r.ni - r.ni/2 + o, r.nj, r.nk,
        (r.ni - r.ni/2 + o) * r.nj * r.nk,
        r.X ? r.X + r.ni/2 - o : NULL,
        r.Y ? r.Y : NULL,
        r.Z ? r.Z : NULL,
        r.L ? r.L : NULL};
}

////////////////////////////////////////////////////////////////////////////////

void bisect_y_overlap(const Region r, Region* const A, Region* const B)
{
    const int o = r.nj/32;

    *A = (Region) {
        r.imin, r.jmin, r.kmin,
        r.ni,   r.nj/2+o, r.nk,
        r.ni*(r.nj/2+o)*r.nk,
        r.X ? r.X : NULL,
        r.Y ? r.Y : NULL,
        r.Z ? r.Z : NULL,
        r.L ? r.L : NULL};
    *B = (Region) {
        r.imin, r.jmin+r.nj/2-o, r.kmin,
        r.ni,   r.nj - r.nj/2+o, r.nk,
        r.ni*(r.nj - r.nj/2+o)*r.nk,
        r.X ? r.X : NULL,
        r.Y ? r.Y + r.nj/2-o: NULL,
        r.Z ? r.Z : NULL,
        r.L ? r.L : NULL};
}

////////////////////////////////////////////////////////////////////////////////

void bisect_z_overlap(const Region r, Region* const A, Region* const B)
{
    const int o = r.nk/32;

    *A = (Region) {
        r.imin, r.jmin, r.kmin,
        r.ni,   r.nj,   r.nk/2+o,
        r.ni*r.nj*(r.nk/2+o),
        r.X ? r.X : NULL,
        r.Y ? r.Y : NULL,
        r.Z ? r.Z : NULL,
        r.L ? r.L : NULL};
    *B = (Region) {
        r.imin, r.jmin, r.kmin+r.nk/2-o,
        r.ni,   r.nj,   r.nk - r.nk/2+o,
        r.ni*r.nj*(r.nk - r.nk/2+o),
        r.X ? r.X : NULL,
        r.Y ? r.Y : NULL,
        r.Z ? r.Z + r.nk/2-o : NULL,
        r.L ? r.L + r.nk/2-o : NULL};
}

////////////////////////////////////////////////////////////////////////////////

// Splits a region in two along its longest axis.  Returns 1
// if the region is of volume 1 and cannot be split.
int bisect(const Region r, Region* const A, Region* const B)
{
    if (r.ni * r.nj * r.nk == 1) {
        *A = r;
        return 1;
    }

    if (r.ni >= r.nj && r.ni >= r.nk)
        bisect_x(r, A, B);
    else if (r.nj >= r.nk)
        bisect_y(r, A, B);
    else
        bisect_z(r, A, B);

    return 0;
}

////////////////////////////////////////////////////////////////////////////////

// Splits a region in two along either the x or y axis
int bisect_xy(const Region r, Region* const A, Region* const B)
{
    if (r.ni * r.nj * r.nk == 1) {
        *A = r;
        return 1;
    }

    if (r.ni >= r.nj && r.ni >= r.nk)
        bisect_x(r, A, B);
    else
        bisect_y(r, A, B);

    return 0;
}

////////////////////////////////////////////////////////////////////////////////

// Splits a region into up to 8 sections, returning
// a bitfield with bits set where a region was stored.
uint8_t octsect(const Region R, Region* const out)
{
    out[0] = R;
    uint8_t bits = 1;

    if (R.nk > 1) {
        bisect_z(out[0], out, out+1);
        bits |= (bits << 1);
    }

    if (R.nj > 1) {
        if (bits & (1 << 0))
            bisect_y(out[0], out,   out+2);
        if (bits & (1 << 1))
            bisect_y(out[1], out+1, out+3);
        bits |= (bits << 2);
    }

    if (R.ni > 1) {
        if (bits & (1 << 0))
            bisect_x(out[0], out,   out+4);
        if (bits & (1 << 1))
            bisect_x(out[1], out+1, out+5);
        if (bits & (1 << 2))
            bisect_x(out[2], out+2, out+6);
        if (bits & (1 << 3))
            bisect_x(out[3], out+3, out+7);
        bits |= (bits << 4);
    }

    return bits;
}

int octsect_active(const Region R, const PackedTree* tree, Region* const out)
{
    out[0] = R;
    uint8_t bits = 1;
    const uint8_t active = active_axes(tree);

    if (R.nk > 1 && (active & 1)) {
        bisect_z(out[0], out, out+1);
        bits |= (bits << 1);
    }

    if (R.nj > 1 && (active & 2)) {
        if (bits & (1 << 0))
            bisect_y(out[0], out,   out+2);
        if (bits & (1 << 1))
            bisect_y(out[1], out+1, out+3);
        bits |= (bits << 2);
    }

    if (R.ni > 1 && (active & 4)) {
        if (bits & (1 << 0))
            bisect_x(out[0], out,   out+4);
        if (bits & (1 << 1))
            bisect_x(out[1], out+1, out+5);
        if (bits & (1 << 2))
            bisect_x(out[2], out+2, out+6);
        if (bits & (1 << 3))
            bisect_x(out[3], out+3, out+7);
        bits |= (bits << 4);
    }

    return bits;
}

////////////////////////////////////////////////////////////////////////////////

// Splits a region into up to 8 sections, returning
// a bitfield with bits set where a region was stored.
uint8_t octsect_overlap(const Region R, Region* const out)
{
    out[0] = R;
    uint8_t bits = 1;

    if (R.nk > 1) {
        bisect_z_overlap(out[0], out, out+1);
        bits |= (bits << 1);
    }

    if (R.nj > 1) {
        if (bits & (1 << 0))
            bisect_y_overlap(out[0], out,   out+2);
        if (bits & (1 << 1))
            bisect_y_overlap(out[1], out+1, out+3);
        bits |= (bits << 2);
    }

    if (R.ni > 1) {
        if (bits & (1 << 0))
            bisect_x_overlap(out[0], out,   out+4);
        if (bits & (1 << 1))
            bisect_x_overlap(out[1], out+1, out+5);
        if (bits & (1 << 2))
            bisect_x_overlap(out[2], out+2, out+6);
        if (bits & (1 << 3))
            bisect_x_overlap(out[3], out+3, out+7);
        bits |= (bits << 4);
    }

    return bits;
}


////////////////////////////////////////////////////////////////////////////////

// Splits a region into up to 8 sections, returning
// a bitfield with bits set where a region was stored.
uint8_t octsect_merged(const Region R, const ASDF* const asdf,
                       Region* const out)
{
    out[0] = R;
    uint8_t bits = 1;

    if (asdf->branches[1] && R.nk > 1) {
        bisect_z(out[0], out, out+1);
        bits |= (bits << 1);
    }

    if (asdf->branches[2] && R.nj > 1) {
        if (bits & (1 << 0))
            bisect_y(out[0], out,   out+2);
        if (bits & (1 << 1))
            bisect_y(out[1], out+1, out+3);
        bits |= (bits << 2);
    }

    if (asdf->branches[4] && R.ni > 1) {
        if (bits & (1 << 0))
            bisect_x(out[0], out,   out+4);
        if (bits & (1 << 1))
            bisect_x(out[1], out+1, out+5);
        if (bits & (1 << 2))
            bisect_x(out[2], out+2, out+6);
        if (bits & (1 << 3))
            bisect_x(out[3], out+3, out+7);
        bits |= (bits << 4);
    }

    return bits;
}

////////////////////////////////////////////////////////////////////////////////


int split(const Region R, Region* const out, const int count)
{
    out[0] = R;
    int num = 1, tail = 1;

    // Keep splitting elements in the list
    while (num < count) {
        for (int i=0; i < num && tail < count; ++i)
            if (bisect(out[i], &out[i], &out[tail++]))
                return tail - 1;
        num = tail;
    }

    return count;
}

////////////////////////////////////////////////////////////////////////////////

int split_xy(const Region R, Region* const out, const int count)
{
    out[0] = R;
    int num = 1, tail = 1;

    // Keep splitting elements in the list
    while (num < count) {
        for (int i=0; i < num && tail < count; ++i)
            if (bisect_xy(out[i], &out[i], &out[tail++]))
                return tail - 1;
        num = tail;
    }

    return count;
}

////////////////////////////////////////////////////////////////////////////////

Region bound_region(const ASDF* const asdf, const Region r)
{
    Region r_ = (Region){ .imin=r.imin, .jmin=r.jmin, .kmin=r.kmin,
                          .ni=1, .nj=1, .nk=1,
                          .X=r.X, .Y = r.Y, .Z=r.Z, .L=r.L};

    // Shrink the region based on the ASDF bounds

    // We'll use loops, because doing it numerically ends up missing
    // by a few pixels on occasion

    if (r.X[r.ni] > r.X[0]) {
        int di;
        for (di = 0; r.X[di + 1] < asdf->X.lower && di < r.ni - 1; ++di);
        r_.imin += di;
        r_.X    += di;
        r_.ni    = r.ni - di;
        for ( ; r_.X[r_.ni - 1] > asdf->X.upper && r_.ni > 1; --r_.ni);
    }

    if (r.Y[r.nj] > r.Y[0]) {
        int dj;
        for (dj = 0; r.Y[dj + 1] < asdf->Y.lower && dj < r.nj - 1; ++dj);
        r_.jmin += dj;
        r_.Y    += dj;
        r_.nj    = r.nj - dj;
        for ( ; r_.Y[r_.nj - 1] > asdf->Y.upper && r_.nj > 1; --r_.nj);

    }

    if (r.Z[r.nk] > r.Z[0]) {
        int dk;
        for (dk = 0; r.Z[dk + 1] < asdf->Z.lower && dk < r.nk - 1; ++dk);
        r_.kmin += dk;
        r_.Z    += dk;
        r_.L    += dk;
        r_.nk = r.nk - dk;
        for ( ; r_.Z[r_.nk - 1] > asdf->Z.upper && r_.nk > 1; --r_.nk);
    }

    r_.voxels = r_.ni*r_.nj*r_.nk;
    return r_;
}


Region rot_bound_region(const struct ASDF_* const asdf,
                        const Region r, const float M[4])
{
    Interval X, Y, Z;
    project_cube(asdf->X, asdf->Y, asdf->Z, &X, &Y, &Z, M);

    // If the region doesn't intersect with the ASDF cell, return a
    // dummy region with voxels set to zero.
    if (X.upper < r.X[0] || X.lower > r.X[r.ni] ||
        Y.upper < r.Y[0] || Y.lower > r.Y[r.nj] ||
        Z.upper < r.Z[0] || Z.lower > r.Z[r.nk])
    {
        return (Region){ .voxels=0 };
    }

    ASDF dummy = (ASDF){ .X=X, .Y=Y, .Z=Z };
    return bound_region(&dummy, r);
}
