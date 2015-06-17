#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "tree/tree.h"
#include "tree/node/node.h"
#include "tree/node/opcodes.h"

#include "util/region.h"

void build_arrays(Region* const R,
                  const float xmin, const float ymin, const float zmin,
                  const float xmax, const float ymax, const float zmax)
{
    const bool has_z = (zmax != zmin) && !isinf(zmax) && !isinf(zmin);

    R->X = malloc((R->ni+1)*sizeof(float));
    for (int i = 0; i <= R->ni; ++i)
        R->X[i] = xmin*(R->ni - i)/(float)R->ni + xmax*i/(float)R->ni;

    R->Y = malloc((R->nj+1)*sizeof(float));
    for (int j = 0; j <= R->nj; ++j)
        R->Y[j] = ymin*(R->nj - j)/(float)R->nj + ymax*j/(float)R->nj;

    R->Z = malloc((R->nk+1)*sizeof(float));
    for (int k = 0; k <= R->nk; ++k)
        R->Z[k] = has_z ? zmin*(R->nk - k)/(float)R->nk + zmax*k/(float)R->nk
                        : 0;

    R->L = malloc((R->nk+1)*sizeof(uint16_t));
    for (int h = 0; h <= R->nk; ++h)
        R->L[h] = has_z ? (65535 * h) / (R->nk) : 65535;
}

void free_arrays(Region* const R)
{
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

int octsect_active(const Region R, const MathTree* tree, Region* const out)
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



